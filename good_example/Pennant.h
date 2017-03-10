/*************************************
 *            Pennant.h              *
 * by Dane Pitkin and Nathan Crandall*
 *        for CS140 Final Project    *
 *      in Parallel BFS Algorithm    *
 *                                   *
 *                                   *
 ************************************/

#ifndef PENNANT_H
#define PENNANT_H

#include <cstddef>

extern "C++" {

  struct Node{
    Node* left;
    Node* right;
    int item;
    
    Node(){
      left = NULL;
      right = NULL;
      item = 0;
    }
    Node(int value){
      left = NULL;
      right = NULL;
      item = value;
    }
  };

  class Pennant{

  public:
    Pennant();
    Pennant(int value);
    ~Pennant();
    Pennant* Pmerge(Pennant* y);
    Pennant* Pmerge_FA(Pennant* x, Pennant* & y); 
    Pennant* Psplit();
    void remove_all(Node* node);
	
    //private:
    Node* root;

  };



}//end extern c++



#endif
