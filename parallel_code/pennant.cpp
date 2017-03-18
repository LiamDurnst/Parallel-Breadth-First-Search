#include "pennant.h"

Pennant::Pennant() {
  this->root = NULL;
}

Pennant::Pennant(int vertex) {
  this->root = new Node(vertex);
}

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
Pennant* Pennant::full_adder(Pennant* x, Pennant* &y) {
  if (x== NULL && y == NULL && this->root == NULL) // 000
    return NULL;
  else if (x == NULL && y == NULL)                 // 100
    return this;
  else if (this->root == NULL && x == NULL)        // 001
    return y;
  else if (this->root == NULL && y == NULL)        // 010
    return x;
  else if (x == NULL){                             // 101

    y = y->pennant_union(this);
    return NULL;
  }
  else if (this->root == NULL){                    // 011

    y = y->pennant_union(x);
    return NULL;
  }
  else if (y == NULL){                             // 110

    y = this->pennant_union(x);
    return NULL;
  }
  else{                                            //

    y = y->pennant_union(x);
    return this;
  }

  //Should never reach here
  return NULL;


  // if (this->root == NULL) {
  //   if (y!=NULL && y->root == NULL) {
  //     if (z!=NULL && z->root == NULL) { // 0 0 0
  //       return NULL;
  //     } else {
  //       return z;            // 0 0 1
  //     }
  //   } else {
  //     if (z!=NULL && z->root == NULL) { // 0 1 0
  //       return y;
  //     } else {               // 0 1 1
  //       z->pennant_union(y);
  //       return NULL;
  //     }
  //   }
  // } else {
  //   if (y!=NULL && y->root == NULL) {
  //     if (z!=NULL && z->root == NULL) { // 1 0 0
  //       return this;
  //     } else {               // 1 0 1
  //       z->pennant_union(this);
  //       return NULL;
  //     }
  //   } else {
  //     if (z!=NULL && z->root == NULL) { // 1 1 0
  //       z = this->pennant_union(y);
  //       return NULL;
  //     } else {               // 1 1 1
  //       z->pennant_union(y);
  //       return this;
  //     }
  //   }
  // }
  // return NULL; // return something of type Pennant
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
