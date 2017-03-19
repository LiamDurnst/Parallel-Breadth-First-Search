#ifndef BAG_H
#define BAG_H

#include "pennant.h"
#include <stdio.h>
#include <math.h>
#include <cilk/cilk.h>
#include <cilk/reducer.h>


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

// class Bag {
//   public:
//     Bag();
//     ~Bag();
//     void bag_insert(int vertex);
//     void bag_union(Bag* bag);
//     void reset();
//     bool is_empty();
//     int n_vertices();
//     Bag* bag_split();
//
//
//     friend class Bag_reducer;
//     friend class cilk::monoid_base<Bag >;
//
//     int backbone_size;
//     Pennant** backbone;
// };
//
// class Bag_reducer {
//   public:
//     struct Monoid: cilk::monoid_base<Bag> {
//       static void reduce(Bag *left, Bag *right){
//         left->bag_union(right);
//       }
//     };
//
//     Bag_reducer() : imp_() {}
//
//     Pennant* get_backbone(int i) {
//       return imp_.view().backbone[i];
//     }
//
//     void bag_insert(int item) {
//       imp_.view().bag_insert(item);
//     }
//
//
//   private:
//     cilk::reducer<Monoid> imp_;
//
// };
//
//
// #endif
