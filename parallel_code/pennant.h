#ifndef PENNANT_H
#define PENNANT_H

#include <stdio.h>

struct Node {
  int vertex;
  Node* left;
  Node* right;

  Node() {
    vertex = 0;
    left = NULL;
    right = NULL;
  }

  Node(int v) {
    vertex = v;
    left = NULL;
    right = NULL;
  }
};

class Pennant {
  public:
    Pennant();
    Pennant(int vertex);

    Pennant* pennant_union(Pennant* y);
    Pennant* full_adder(Pennant* x, Pennant* &y);
    Pennant* pennant_split();

    Node* root;
};

#endif
