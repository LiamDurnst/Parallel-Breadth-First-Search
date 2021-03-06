/*************************************
 *              Bag.h                *
 * by Dane Pitkin and Nathan Crandall*
 *        for CS140 Final Project    *
 *      in Parallel BFS Algorithm    *
 *                                   *
 *                                   *
 ************************************/



#ifndef BAG_H
#define BAG_H

#include "Pennant.h"
#include <cilk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern "C++"{

class Bag{

 public:
  Bag();
  Bag(const Bag* & bag);
  ~Bag();

  void insert(int item);
  bool empty() const;
  void merge(Bag* bag);
  Bag* split();
  void clear();
  int size() const;
  bool can_split() const;
  void print() const;
  void recursive_print(Node* node) const; // prints in-order
  void recursive_print_sum(Node* node, int & sum) const;
  void remove_all();
  int* write_array();
  void recursive_write_array(int* & array, Node* node, int &count);

  friend class Bag_reducer;
  friend class cilk::monoid_base<Bag >;

  //private:
  int forest_size;
  //pennant structure
  Pennant** forest;
};


  // Make the bag a reducer hyperobject
class Bag_reducer{
  
 public:
  struct Monoid: cilk::monoid_base<Bag >
  {
    static void reduce(Bag *left, Bag *right){
      left->merge(right);
    }
  };

  Bag_reducer() : imp_() {}
  int get_forest_size()
  {
    return imp_.view().forest_size;
  }
  Pennant* get_forest(int i)
  {
    return imp_.view().forest[i];
  }
  void set_forest(int i, Pennant* tree)
  {
    imp_.view().forest[i] = tree;
  }
  void insert(int item)
  {
  imp_.view().insert(item);
  }
  void merge(Bag_reducer* br)
  {
  this->imp_.view().merge(&br->imp_.view());
  }
  Bag* split()
  {
    return imp_.view().split();
  }
  bool empty() const
  {
  return imp_.view().empty();
  }
  void clear()
  {
  return imp_.view().clear();
  }
  Bag &get_reference()
  {
  return imp_.view();
  }
  int size()
  {
    return imp_.view().size();
  }
  int* write_array()
  {
    return imp_.view().write_array();
  }


 private:
  cilk::reducer<Monoid> imp_;

};



}//end extern c++

#endif
