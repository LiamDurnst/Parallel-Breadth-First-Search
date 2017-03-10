/*************************************
 *            Pennant.cpp            *
 * by Dane Pitkin and Nathan Crandall*
 *        for CS140 Final Project    *
 *      in Parallel BFS Algorithm    *
 *                                   *
 *                                   *
 ************************************/


#include "Pennant.h"

extern "C++" {
  
  Pennant::Pennant()
  {
    this->root = NULL;
  }

  Pennant::Pennant(int value)
  {
    this->root = new Node(value);
  }

  Pennant::~Pennant()
  {
    
  }
  
  Pennant* Pennant::Pmerge(Pennant* y)
  {
    if (this->root == NULL){
      return y;
    }
    else
    {
      this->root->right = y->root->left;
      y->root->left = this->root;
    }

    return y;
  }
  
  //Pmerge_FA: Pennant Merge Full Adder. Concept: Pointers 'this' and 'x' are 
  //two bits being added together, 'y' is carry bit. 
  Pennant* Pennant::Pmerge_FA(Pennant* x, Pennant* & y)
  {
    if (x== NULL && y == NULL && this->root == NULL)
      return NULL;
    else if (x == NULL && y == NULL)
      return this;
    else if (this->root == NULL && x == NULL)
      return y;
    else if (this->root == NULL && y == NULL)
      return x;
    else if (x == NULL){

      y = y->Pmerge(this);
      return NULL;
    }
    else if (this->root == NULL){

      y = y->Pmerge(x);
      return NULL;
    }
    else if (y == NULL){

      y = this->Pmerge(x);
      return NULL;
    }
    else{

      y = y->Pmerge(x);
      return this;
    }
    
    //Should never reach here
    return NULL;
  }

  //Splits pennant. 
  //Returns Pennant* containing other half of pennant
  Pennant* Pennant::Psplit()
  {
    if(this->root != NULL && this->root->left != NULL){
      Pennant* y = new Pennant();
      y->root = this->root->left;
      this->root->left = y->root->right;
      y->root->right = NULL;
      return y;
    }
    return NULL;
  }
  
  void Pennant::remove_all(Node* node)
  {
    if (node->left != NULL)
      remove_all(node->left);
    if (node->right != NULL)
      remove_all(node->right);
    delete node;
  }

}//end extern c++
