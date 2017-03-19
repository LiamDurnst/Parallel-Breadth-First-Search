#include "pennant.h"

Pennant::Pennant() {
  this->root = NULL;
}

Pennant::Pennant(int vertex) {
  this->root = new Node(vertex);
}

Pennant::~Pennant() {}

Pennant* Pennant::pennant_union(Pennant* y) {
  if(y==NULL){
    return this;
  }
  // if x is empty return y
  else if (this->root == NULL) {
    return y;
  }
  // if y is empty return x
  else if (y->root == NULL) {
    return this;
  }

  // if neither are empty
  y->root->right = this->root->left;
  this->root->left = y->root;
  return this;
}

// full_adder is a helper function for bag union
Pennant* Pennant::full_adder(Pennant* y, Pennant* &z) {
  if (this->root == NULL) {
    if (y == NULL) {
      if (z == NULL) {                  // 0 0 0
        return NULL;
      } else {
        return z;                       // 0 0 1
      }
    } else {
      if (z == NULL) {                  // 0 1 0
        return y;
      } else {                          // 0 1 1
        z->pennant_union(y);
        return NULL;
      }
    }
  } else {
    if (y == NULL) {
      if (z == NULL) {                  // 1 0 0
        return this;
      } else {                          // 1 0 1
        z->pennant_union(this);
        return NULL;
      }
    } else {
      if (z == NULL) {                  // 1 1 0
        z = this->pennant_union(y);
        return NULL;
      } else {                          // 1 1 1
        z->pennant_union(y);
        return this;
      }
    }
  }
  return NULL; // return something of type Pennant
}

// only split if input pennant has at least 2 elements
Pennant* Pennant::pennant_split() {
  if (this->root != NULL && this->root->left != NULL) {
    Pennant* y = new Pennant();
    y->root = this->root->left;
    this->root->left = y->root->right;
    y->root->right = NULL;
    return y;
  }
  return NULL;
}

void Pennant::remove_all(Node* node) {
  if (node->left != NULL) remove_all(node->left);
  if (node->right != NULL) remove_all(node->right);
  delete node;
}
