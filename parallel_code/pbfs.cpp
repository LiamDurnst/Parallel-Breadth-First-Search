// Ricky Kuang
// Josue Montenegro
// CS140 Final Project

/* Code provided by Prof. John Gilbert */

#include "bag.h"

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <sys/time.h>   // timer

using namespace std;

typedef struct graphstruct { // A graph in compressed-adjacency-list (CSR) form
  int nv;            // number of vertices
  int ne;            // number of edges
  int *nbr;          // array of neighbors of all vertices
  int *firstnbr;     // index in nbr[] of first neighbor of each vtx
} graph;


int read_edge_list (int **tailp, int **headp) {
  int max_edges = 1000000;
  int nedges, nr, t, h;
  *tailp = (int *) calloc(max_edges, sizeof(int));
  *headp = (int *) calloc(max_edges, sizeof(int));
  nedges = 0;
  nr = scanf("%i %i",&t,&h);
  while (nr == 2) {
    if (nedges >= max_edges) {
      printf("Limit of %d edges exceeded.\n",max_edges);
      exit(1);
    }
    (*tailp)[nedges] = t;
    (*headp)[nedges++] = h;
    nr = scanf("%i %i",&t,&h);
  }
  return nedges;
}


graph * graph_from_edge_list (int *tail, int* head, int nedges) {
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


void walk_bag(graph* G, Node* root, Bag_reducer* &out_bag, int thislevel, int* & level, int* & parent) {
  if (root == NULL)
    return;

  int current_node = root->vertex;
  int end = G->firstnbr[current_node + 1];
  //cout << "Current node: " << current_node << endl;
  for (int u = G->firstnbr[current_node]; u < end; u++) { // cilk_for
    int current_neighbor = G->nbr[u];
    //cout<< "current_neighbor: " << current_neighbor << endl;
    if (level[current_neighbor] == -1) {
      parent[current_neighbor] = current_node;
      level[current_neighbor] = thislevel + 1;
      out_bag->bag_insert(current_neighbor);
    }
  }

  walk_bag(G, root->left, out_bag, thislevel, level, parent); // cilk_spawn
  walk_bag(G, root->right, out_bag, thislevel, level, parent);
}


void process_layer(graph* G, Bag* &in_bag, Bag_reducer* &out_bag,int thislevel, int* & level, int* & parent) {
  // if BAG_SIZE(in_bag) < GRAINSIZE
  //   for each u in in_bag
  //     parallel for each v in Adj[u]
  //       if v.dist == -1
  //         v.dist = d+1                  // benign race
  //         BAG_INSERT(out_bag, v)
  //   return
  // new_bag = BAG_SPLIT(in_bag)
  // spawn PROCESS_LAYER(new_bag, out_bag, d)
  // PROCESS_LAYER(in_bag, out_bag, d)
  // sync
  //cout << "backbone[2]: "<< in_bag->backbone[2]->root << endl;
  if (in_bag->n_vertices() < 128) {
    cilk_for(int i = 0; i < in_bag->n_vertices(); i++) {
      //cout << "before walk_bag ITER: " << i << endl;
      if(in_bag->backbone[i]!=NULL)
        walk_bag(G, in_bag->backbone[i]->root, out_bag, thislevel, level, parent);
      //cout << "after walk_bag ITER: " << i << endl;
    }
    return;
  }
  Bag* new_bag = in_bag->bag_split();
  cilk_spawn process_layer(G,new_bag, out_bag, thislevel, level, parent);
  process_layer(G, in_bag, out_bag, thislevel, level,parent);
  cilk_sync;
}


void pbfs(int s, graph *G, int **levelp, int *nlevelsp, int **levelsizep, int **parentp) {
  // parallel for each vertex v in V(G) - {v_0}
  //   v.dist = -1
  // v_0.dist = 0
  // d = 0
  // v_0 = BAG_CREATE()
  // BAG_INSERT(V_0, v_0)
  // while !BAG-IS-EMPTY(V_d)
  //   V_d+1 = new reducer BAG_CREATE()
  //   PROCESS_LAYER(revert V_d, V_d+1, d)
  //   d = d+1

  int *level, *levelsize, *parent;
  int thislevel;

  level = *levelp = (int *) calloc(G->nv, sizeof(int));
  levelsize = *levelsizep = (int *) calloc(G->nv, sizeof(int));
  parent = *parentp = (int *) calloc(G->nv, sizeof(int));

  for (int v = 0; v < G->nv; v++) level[v] = -1;
  for (int v = 0; v < G->nv; v++) parent[v] = -1;

  cout << "after init level and parent" << endl;

  // assign the starting vertex level 0 and put it on the queue to explore
  thislevel = 0;
  level[s] = 0;
  levelsize[0] = 1;
  Bag* bag = new Bag();
  bag->bag_insert(s);

  cout << "after init bag" << endl;

  while (!bag->is_empty()) {
    levelsize[thislevel] = bag->n_vertices();
    Bag_reducer* out_bag = new Bag_reducer();

    cout << "before process_layer" << endl;

    process_layer(G,bag,out_bag,thislevel,level,parent);

    cout << "after process_layer" << endl;

    thislevel++;
    //we want to reset our bag to be fresh in our next iteration of process layer
    bag->reset();
    //copy nodes we got from previous process_layer iteration into our cleared bag
    for(int i=0; i<bag->backbone_size; i++){
      if(out_bag->get_backbone(i)!=NULL)
        bag->backbone[i] = out_bag->get_backbone(i);
    }
  }
  *nlevelsp = thislevel;
}


int main(int argc, char* argv[]) {
  graph *G;
  int *level, *levelsize, *parent;
  int *tail, *head;
  int nedges;
  int nlevels;
  int startvtx;
  int i, v, reached;

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

  printf("Starting vertex for BFS is %d.\n\n", startvtx);


  struct timeval tv1, tv2;
  double duration;

  gettimeofday(&tv1, NULL);
  pbfs(startvtx, G, &level, &nlevels, &levelsize, &parent);
  gettimeofday(&tv2, NULL);

  double start, end;
  start = (double)tv1.tv_sec + (double)tv1.tv_usec/1000000.0;
  end = (double)tv2.tv_sec + (double)tv2.tv_usec/1000000.0;

  duration = end - start;
  cout << "Time in seconds: " << duration << " seconds" << endl;


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
