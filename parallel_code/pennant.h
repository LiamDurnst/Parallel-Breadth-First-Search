#ifndef PENNANT_H
#define PENNANT_H

#include <stdio.h>

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


// struct Node {
//   int vertex;
//   Node* left;
//   Node* right;
//
//   Node() {
//     vertex = 0;
//     left = NULL;
//     right = NULL;
//   }
//
//   Node(int v) {
//     vertex = v;
//     left = NULL;
//     right = NULL;
//   }
// };
//
// class Pennant {
//   public:
//     Pennant();
//     Pennant(int vertex);
//     ~Pennant();
//
//     Pennant* pennant_union(Pennant* y);
//     Pennant* full_adder(Pennant* y, Pennant* &z);
//     Pennant* pennant_split();
//     void remove_all(Node* node);
//
//     Node* root;
// };
//
// #endif
