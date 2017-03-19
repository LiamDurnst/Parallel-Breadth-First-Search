#ifndef BAG_H
#define BAG_H

#include "pennant.h"
#include <stdio.h>
#include <math.h>
#include <cilk/cilk.h>
#include <cilk/reducer.h>


class Bag {
  public:
    Bag();
    void bag_insert(int vertex);
    void bag_union(Bag* bag);
    void reset();
    bool is_empty();
    int n_vertices();
    Bag* bag_split();


    friend class Bag_reducer;
    friend class cilk::monoid_base<Bag >;

    int backbone_size;
    Pennant** backbone;
};

class Bag_reducer {
  public:
    struct Monoid: cilk::monoid_base<Bag> {
      static void reduce(Bag *left, Bag *right){
        left->bag_union(right);
      }
    };

    Bag_reducer() : imp_() {}

    // int get_backbone_size() {
    //   return imp_.view().backbone_size;
    // }

    Pennant* get_backbone(int i) {
      return imp_.view().backbone[i];
    }

    // void set_backbone(int i, Pennant* tree) {
    //   imp_.view().backbone[i] = tree;
    // }

    void bag_insert(int item) {
      imp_.view().bag_insert(item);
    }

    // void bag_union(Bag_reducer* that) {
    //   this->imp_.view().bag_union(&that->imp_.view());
    // }
    //
    // Bag* bag_split() {
    //   return imp_.view().bag_split();
    // }
    //
    // bool is_empty() const {
    //   return imp_.view().is_empty();
    // }
    //
    // int n_vertices() {
    //   return imp_.view().n_vertices();
    // }

  private:
    cilk::reducer<Monoid> imp_;

};


#endif
