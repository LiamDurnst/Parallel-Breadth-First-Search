#ifndef PENNANT_H
#define PENNANT_H

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
  ~Pennant();

  Pennant* pennant_union();
  Pennant* pennant_split();
};

#endif
