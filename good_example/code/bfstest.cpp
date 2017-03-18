// BFSTEST : Test breadth-first search in a graph.
// 
// example: cat sample.txt | ./bfstest 1
//
// John R. Gilbert, 17 Feb 2011

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bag.h"
#include <set>
#include <vector>
#include <cilkview.h>
#include <algorithm>
#include <sys/time.h>
using namespace std;

typedef struct graphstruct { // A graph in compressed-adjacency-list (CSR) form
	int nv;            // number of vertices
	int ne;            // number of edges
	int *nbr;          // array of neighbors of all vertices
	int *firstnbr;     // index in nbr[] of first neighbor of each vtx
} graph;


int read_edge_list (int **tailp, int **headp, const char *fname) {
// int max_edges = 20000000; // 20 mil, atm
 int max_edges = 1300000000; // 1.3 billion. Let's go, baby.
  int nedges, nr, t, h;
  FILE *fp = fopen(fname, "r");
  *tailp = (int *) calloc(max_edges, sizeof(int));
  *headp = (int *) calloc(max_edges, sizeof(int));
  nedges = 0;
  nr = fscanf(fp, "%i %i",&t,&h);
  while (nr == 2) {
    if (nedges >= max_edges) {
      printf("Limit of %d edges exceeded.\n",max_edges);
      exit(1);
    }
    (*tailp)[nedges] = t;
    (*headp)[nedges++] = h;
    nr = fscanf(fp, "%i %i",&t,&h);
  }
  return nedges;
}
void sortEdges(int *begin, int* end)
{
  if(begin != end)
  {
    --end;
    int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
    std::swap(*end, *middle);
    cilk_spawn sortEdges(begin, middle);
    sortEdges(++middle, ++end);
    cilk_sync;
  }
}

graph * graph_from_edge_list (int *tail, int* head, int nedges) {
  /*
  cilk::cilkview cv;
  cv.start();
  */
  graph *G;
  int i, e, v, maxv;
  G = (graph *) calloc(1, sizeof(graph));
  G->ne = nedges;
  maxv = 0;

  // count vertices
  for (e = 0; e < G->ne; e++) {
    if (tail[e] > maxv) maxv = tail[e];
    if (head[e] > maxv) maxv = head[e];
  }
  G->nv = maxv+1;
  G->nbr = (int *) calloc(G->ne, sizeof(int));
  G->firstnbr = (int *) calloc(G->nv+1, sizeof(int));

  // count neighbors of vertex v in firstnbr[v+1],
  for (e = 0; e < G->ne; e++) G->firstnbr[tail[e]+1]++;

  // cumulative sum of neighbors gives firstnbr[] values
  for (v = 0; v < G->nv; v++) G->firstnbr[v+1] += G->firstnbr[v];

  // pass through edges, slotting each one into the CSR structure
  for (e = 0; e < G->ne; e++) {
    i = G->firstnbr[tail[e]]++;
    G->nbr[i] = head[e];
  }

  // the loop above shifted firstnbr[] left; shift it back right
  for (v = G->nv; v > 0; v--) G->firstnbr[v] = G->firstnbr[v-1];
  G->firstnbr[0] = 0;

  //printf("here\n");
  //for(v = 0; v < G->nv - 1; v++)
    //printf("firstnbr[v] %d\n", G->firstnbr[v]);
  for(v = 0; v < G->nv - 1; v++){
     int begin = G->firstnbr[v];
     int end = G->firstnbr[v+1]-1 < begin ? begin : G->firstnbr[v+1]-1;
     //printf("sorting from %d to %d\n", begin, end);
     cilk_spawn sortEdges(G->nbr + begin, G->nbr + end);
  }
  cilk_sync;
  //printf("here2\n");
  //cv.stop();
  //cv.dump("gen graph");
  return G;
}


void print_CSR_graph (graph *G) {
  int vlimit = 20;
  int elimit = 50;
  int e,v;
  printf("\nGraph has %d vertices and %d edges.\n",G->nv,G->ne);
  printf("firstnbr =");
  if (G->nv < vlimit) vlimit = G->nv;
  for (v = 0; v <= vlimit; v++) printf(" %d",G->firstnbr[v]);
  if (G->nv > vlimit) printf(" ...");
  printf("\n");
  printf("nbr =");
  if (G->ne < elimit) elimit = G->ne;
  for (e = 0; e < elimit; e++) printf(" %d",G->nbr[e]);
  if (G->ne > elimit) printf(" ...");
  printf("\n\n");
}

void search_DandC(_bag *last_frontier, cilk::reducer<bag> *red, int thislevel, graph *G, int *level, int *parent, int *levelsize, int grain)
{
	// Divide
	if(last_frontier->size() >= grain)
	{
		cilk_spawn search_DandC(last_frontier->split(), red, thislevel, G, level, parent, levelsize, grain);
		search_DandC(last_frontier, red, thislevel, G, level, parent, levelsize, grain);
		return;
	}

	// Conquer
	vector<int> nodes = last_frontier->dump();

	for(vector<int>::iterator it = nodes.begin();
			it != nodes.end();
			it++)
	{
		int v = *it;
		for(int e = G->firstnbr[v]; e < G->firstnbr[v+1]; e++)
		{
			int node = G->nbr[e];
			if(level[node] == -1)
			{
				parent[node] = v;
				level[node] = thislevel + 1;
				red->view()->insert(node);
			}
		}
	}
}

void bfs (int s, graph *G, int **levelp, int *nlevelsp,
		int **levelsizep, int **parentp) {
	int *level, *levelsize, *parent;
	int thislevel;
	int *queue, back, front;

	// To reduce cache misses, have a bitvector that we can use
	// to de-duplicate (instead of checking the level)
	level = *levelp = (int *) calloc(G->nv, sizeof(int));
	levelsize = *levelsizep = (int *) calloc(G->nv, sizeof(int));
	parent = *parentp = (int *) calloc(G->nv, sizeof(int));
	
	// initially, queue is empty, all levels and parents are -1
	back = 0;   // position next element will be added to queue
	front = 0;  // position next element will be removed from queue
	int v;
	for (v = 0; v < G->nv; v++) level[v] = -1;
	for (v = 0; v < G->nv; v++) parent[v] = -1;

	// assign the starting vertex level 0 and put it on the queue to explore
	thislevel = 0;
	level[s] = 0;
	levelsize[0] = 1;

	// loop over levels, then over vertices at this level, then over neighbors
	cilk::reducer<bag> *reducer = new cilk::reducer<bag>();
	reducer->view() = new _bag(vector<int>());
	reducer->view()->insert(s);
	/*
	printf("Zeroth is null? %s\n", reducer->view()->arr[0] == NULL ? "true" : "false");
	printf("Its value? %d\n", reducer->view()->arr[0]->val);
	printf("Its depth? %d\n", reducer->view()->arr[0]->k);
	*/
	cilk::reducer<bag> *newReducer;
/*
	cilk::cilkview cv;
	cv.start();
*/

	while (levelsize[thislevel] > 0) {
		newReducer = new cilk::reducer<bag>();
		newReducer->view() = new _bag(vector<int>());
		//printf("level: %d size: %d\n", thislevel, levelsize[thislevel]);
		levelsize[thislevel+1] = 0;

		search_DandC(reducer->view(), newReducer, thislevel, G, level, parent, levelsize, 8);

		//Delete the old set
		reducer->view()->cleanup();
		cilk::reducer<bag> *temp = reducer;
		reducer = newReducer;
		newReducer = temp;

		levelsize[thislevel+1] = reducer->view()->size();
		printf("Done with level %d; it has %d unique members.\n", thislevel, levelsize[thislevel+1]);

		thislevel++;
	}

/*
	cv.stop();
	cv.dump("bfs portion");
*/
	*nlevelsp = thislevel;

	reducer->view()->cleanup();
	delete reducer;
	delete newReducer;
}


int cilk_main (int argc, char* argv[]) {
  graph *G;
  int *level, *levelsize, *parent;
  int *tail, *head;
  int nedges;
  int nlevels;
  int startvtx;
  int i, v, reached;

  /* Original code is unmodified here
  if (argc == 2) {
    startvtx = atoi (argv[1]);
  } else {
    printf("usage:   bfstest <startvtx> < <edgelistfile>\n");
    printf("example: cat sample.txt | ./bfstest 1\n");
    exit(1);
  }
  nedges = read_edge_list (&tail, &head);
  G = graph_from_edge_list (tail, head, nedges);
  free(tail);
  free(head);
  print_CSR_graph (G);
  */

	/* New code starts here ************************/
	if(argc != 3)
	{
		printf("Usage: %s <startvtx> <inputfile>\n", argv[0]);
		exit(1);
	}
	startvtx = atoi(argv[1]);
	nedges = read_edge_list(&tail, &head, argv[2]);
	G = graph_from_edge_list(tail, head, nedges);
	free(tail);
	free(head);
	print_CSR_graph(G);


  printf("Starting vertex for BFS is %d.\n\n",startvtx);

cilk::cilkview cv;
cv.start();
  bfs (startvtx, G, &level, &nlevels, &levelsize, &parent);
//  printf("time: %lf\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0);
cv.stop();
printf("Time: %d ms.\n", cv.accumulated_milliseconds());
//cv.dump("new");

  reached = 0;
  for (i = 0; i < nlevels; i++) reached += levelsize[i];
  printf("Breadth-first search from vertex %d reached %d levels and %d vertices.\n",
    startvtx, nlevels, reached);
  for (i = 0; i < nlevels; i++) printf("level %d vertices: %d\n", i, levelsize[i]);
  if (G->nv < 20) {
    printf("\n  vertex parent  level\n");
    for (v = 0; v < G->nv; v++) printf("%6d%7d%7d\n", v, parent[v], level[v]);
  }
  printf("\n");
}




