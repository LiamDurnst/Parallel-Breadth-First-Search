#ifndef BAG_H
#define BAG_H

#include "pennant.h"
#include <cilk.h>

class Bag {
  Bag();

  void bag_insert(int vertex);
  void bag_union(Bag* bag);
  Bag* bag_split();

  int backbone_size;
  Pennant** backbone;
};

#endif
