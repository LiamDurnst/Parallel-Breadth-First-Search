//Graph Viewer - Dane Pitkin and Nathan Crandall for CS140 Final Project W12
//all code in here was written by us except for the struct graphstruct, 
//and the functions read_edge_list, graph_from_edge_list, and bfs. written by John R. Gilbert, 17 Feb 2011

#include <iostream>
#include <cstdio>
#include <time.h>
#include <ctime>
#include <vector>
#include <sys/time.h>
#include <GL/glut.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

void ProcessKeyboard(unsigned char key, int x, int y);
void Render();
void changeSize(int w, int h);
void arrow_keys(int key, int x, int y);
void renderBitmapString(float x,float y,float z,void *font,char *string);

GLfloat LightAmbient[]=  { 0.0f, 1.0f, 0.0f, 1.0f };    // Ambient Light Values
GLfloat LightDiffuse[]=  { 1.0f, 1.0f, 1.0f, 1.0f };    // Diffuse Light Values
GLfloat LightPosition[]= { -20.0f, 40.0f, 20.0f, 1.0f };    // Light Position


class vector3
{
public:
  float x,y,z;
  vector3(){x = 0.0f; y = 0.0f; z = 0.0f;}
} camera, lookAt;

class Node
{
public:
  vector3 position;
  vector<vector3> neighbors;
  int visited;
  int id;
  

  Node()
  {
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;
    visited = 0;

  }
  Node(float x,float y,float z, int v)
  {
    position.x = x;
    position.y = y;
    position.z = z;
    visited = v;
    
  }
  void setPosition(float x, float y, float z)
  {
    position.x = x;
    position.y = y;
    position.z = z;
  }
  void addNeighbor(vector3 add)
  {
    neighbors.push_back(add);
  }
  void Draw()
  {
    glPushMatrix();
    if(visited == 0) 
      glColor3f(0,1,0);
    else if(visited == 1) 
      glColor3f(0,0,1);
    else 
      glColor3f(1,0,0);
    glTranslatef(position.x,position.y,position.z);
    //draw here
    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluSphere(quadric, 0.5f, 32, 32);
    gluDeleteQuadric(quadric);
    glPopMatrix();
    
    glDisable(GL_LIGHTING);
    for(int i = 0; i < neighbors.size(); i++)
    {
      glPushMatrix();
      glColor3f(1,1,1);
      glBegin(GL_LINES);
      glVertex2f(position.x,position.y);
      glColor3f(1,0,0);
      glVertex2f(neighbors[i].x,neighbors[i].y);
      glEnd();
      glPopMatrix();
    }
    glPushMatrix();
    char id_tos[7];
    sprintf(id_tos, "%d", id);
    id_tos[6] = '\0';
    glColor3f(1,1,0);
    renderBitmapString(position.x,position.y+0.75f,position.z,GLUT_BITMAP_TIMES_ROMAN_24,id_tos); 
    glPopMatrix();
    glEnable(GL_LIGHTING);
  }
  ~Node(){;}
}*nodes;

Node find(int arg_id, Node *array, int n)
{
  for(int i = 0; i < n; i++)
  {
    if(array[i].id == arg_id)
      return array[i];
  }
}

typedef struct graphstruct { // A graph in compressed-adjacency-list (CSR) form
  int nv;            // number of vertices
  int ne;            // number of edges
  int *nbr;          // array of neighbors of all vertices
  int *firstnbr;     // index in nbr[] of first neighbor of each vtx
} graph;

graph *G;

void renderBitmapString(float x,float y,float z,void *font,char *string) 
{
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

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


void bfs (int s, graph *G, int **levelp, int *nlevelsp, 
         int **levelsizep, int **parentp) {
  int *level, *levelsize, *parent;
  int thislevel;
  int *queue, back, front;
  int i, v, w, e;
  level = *levelp = (int *) calloc(G->nv, sizeof(int));
  levelsize = *levelsizep = (int *) calloc(G->nv, sizeof(int));
  parent = *parentp = (int *) calloc(G->nv, sizeof(int));
  queue = (int *) calloc(G->nv, sizeof(int));

  // initially, queue is empty, all levels and parents are -1
  back = 0;   // position next element will be added to queue
  front = 0;  // position next element will be removed from queue
  for (v = 0; v < G->nv; v++) level[v] = -1;
  for (v = 0; v < G->nv; v++) parent[v] = -1;

  // assign the starting vertex level 0 and put it on the queue to explore
  thislevel = 0;
  level[s] = 0;
  levelsize[0] = 1;
  queue[back++] = s;

  // loop over levels, then over vertices at this level, then over neighbors
  while (levelsize[thislevel] > 0) {
    levelsize[thislevel+1] = 0;
    for (i = 0; i < levelsize[thislevel]; i++) {
      v = queue[front++];       // v is the current vertex to explore from
      for (e = G->firstnbr[v]; e < G->firstnbr[v+1]; e++) {
        w = G->nbr[e];          // w is the current neighbor of v
        if (level[w] == -1) {   // w has not already been reached
          parent[w] = v;
          level[w] = thislevel+1;
          levelsize[thislevel+1]++;
          queue[back++] = w;    // put w on queue to explore
        }
      }
    }
    thislevel = thislevel+1;
  }
  *nlevelsp = thislevel;
  free(queue);
}

int main(int argc, char *argv[])
{
  camera.z = 40.0f;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(800,600);
  
  glutCreateWindow("Graph Viewer");
  glEnable(GL_DEPTH_TEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Setup The Ambient Light
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Setup The Diffuse Light
  glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light
  glEnable(GL_LIGHT1);								// Enable Light One
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
  bfs (startvtx, G, &level, &nlevels, &levelsize, &parent);
  nodes = new Node[G->nv];
  
  for(i = 0; i < nlevels; i++)
  {
    for(int j = 0; j < levelsize[i]; j++)
    {
      int n = 0;
      if(i == 0)
	n = j;
      else
      {
	for(int k = i-1; k >= 0; k--)
	  n += levelsize[k];
	n += j;
      }
      nodes[n].id = n;
      
      if(levelsize[i]%2 == 0)
      {
	
	nodes[n].setPosition((-levelsize[i]-1)+((j+1)*2.0f), 15-(2.0f*i), 0);
	
      }
      else
      {
	int count = levelsize[i]/2;	
	if(j == count)
	  nodes[n].setPosition(0.0f, 15-(2.0f*i), 0);
	else if(j < count)
	  nodes[n].setPosition((-count*2.0f)+(j*2.0f), 15-(2.0f*i), 0);
	else if(j > count)
	  nodes[n].setPosition((count*2.0f)-(((j-1)-count)*2.0f), 15-(2.0f*i), 0);
      }
      
    }
  }
  bool added_zero = false;
  //now add neighbors
  for(i = 0; i < G->nv+1; i++)
  {
    if(i != G->nv)
    {
      int num = G->firstnbr[i+1] - G->firstnbr[i];
      if(num != 0)
      {
	for(int j = G->firstnbr[i]; j < num+G->firstnbr[i]; j++)
	{
	  if(j < G->nv)
	  {
	    
	    if(!added_zero || nodes[j].id != 0)
	      nodes[i].addNeighbor(nodes[j].position);
	    if(nodes[j].id == 0)
	      added_zero = true;
	  }
	}
      }
    }
  }
  
  glutDisplayFunc(Render);
  glutIdleFunc(Render);
  glutReshapeFunc(changeSize);  
  glutKeyboardFunc(ProcessKeyboard); 
  glutSpecialFunc(arrow_keys);
  glutMainLoop();
  delete [] nodes;
  return 0; 
}


void ProcessKeyboard(unsigned char key,int x,int y)
{
  if(key == 27)
    exit(0);
  if(key == 'w')
    camera.z -= 0.5f;
  if(key == 's')
    camera.z += 0.5f;
  if(key == 'a')
    camera.x -= 0.5f;
  if(key == 'd')
    camera.x += 0.5f;
}

void arrow_keys(int key, int x, int y)
{
  if(key == GLUT_KEY_UP)
  {
    camera.y += 0.5f;
    lookAt.y += 0.5f;
  }
  if(key == GLUT_KEY_DOWN)
  {
    camera.y -= 0.5f;
    lookAt.y -= 0.5f;
  }
  if(key == GLUT_KEY_LEFT)
  {  
    camera.x -= 0.5f;
    lookAt.x -= 0.5f;
  }
  if(key == GLUT_KEY_RIGHT)
  {
    camera.x += 0.5f;
    lookAt.x += 0.5f;
  }
}

void Render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0,0,0,0);
  glLoadIdentity();

  gluLookAt(camera.x,camera.y,camera.z,lookAt.x,lookAt.y,lookAt.z,0,1,0);

  for(int z = 0; z < G->nv; z++)
    if(z == 0 || nodes[z].id != 0)
      nodes[z].Draw();

  glFlush();
  glutSwapBuffers();
}
void changeSize(int w,int h)
{
  if(h == 0)
    h = 1;

  float ratio = 1.0* w / h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
	
  glViewport(0, 0, w, h);
    
  gluPerspective(45,ratio,1,1000);;
  glMatrixMode(GL_MODELVIEW);
}
