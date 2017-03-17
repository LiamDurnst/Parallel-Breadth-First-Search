#ifndef PENNANT_H
#define PENNANT_H

#include <cstddef>

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
}

class Pennant {
  Pennant();
  Pennant(int vertex);

  static Pennant* pennant_union(Pennant* x, Pennant* y);
  static Pennant* full_adder(Pennant* x, Pennant* y, Pennant* &z);
  Pennant* pennant_split();

  Node* root;
};

#endif
