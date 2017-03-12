#include "pennant.h"

Pennant::Pennant() {
  this->root = NULL;
}

Pennant::Pennant(int vertex) {
  this->root = new Node(vertex);
}

static Pennant* Pennant::pennant_union(Pennant* x, Pennant* y) {
  // if x is empty return y
  if (x->root == NULL) {
    return y;
  }
  // if y is empty return x
  else if (y->root == NULL) {
    return this;
  }
  else {
    y->root->right = x->root->left;
    x->root->left = y->root;
    return x;
  }
}

// full_adder is a helper function for bag union
static Pennant* Pennant::full_adder(Pennant* x, Pennant* y, Pennant* &z) {
  if (x->root == NULL) {
    if (y->root == NULL) {
      if (z->root == NULL) { // 0 0 0
        return NULL;
      } else {
        return z;            // 0 0 1
      }
    } else {
      if (z->root == NULL) { // 0 1 0
        return y;
      } else {               // 0 1 1
        z = pennant_union(y, z)
        return NULL;
      }
    }
  } else {
    if (y->root == NULL) {
      if (z->root == NULL) { // 1 0 0
        return x;
      } else {               // 1 0 1
        z = pennant_union(x, z);
        return NULL;
      }
    } else {
      if (z->root == NULL) { // 1 1 0
        z = pennant_union(x, y);
        return NULL;
      } else {               // 1 1 1
        z = pennant_union(y, z);
        return x;
      }
    }
  }
  return NULL; // return something of type Pennant
}

// only split if input pennant has at least 2 elements
Pennant* Pennant::pennant_split() {
  if (this->root != NULL && this->root->left != NULL) {
    Pennant* y = new Pennant();
    y = this->root->left;
    this->root->left = y->root->right;
    y->root->right = NULL;
    return y;
  } else {
    return NULL;
  }
}
