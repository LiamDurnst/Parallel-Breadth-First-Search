#ifndef BAG_H
#define BAG_H

#include "pennant.h"

class Bag {
  Bag();
  ~Bag();

  void bag_insert();
  void bag_union(Bag* bag);
  Bag* bag_split();
  int bag_size();
};

#endif
