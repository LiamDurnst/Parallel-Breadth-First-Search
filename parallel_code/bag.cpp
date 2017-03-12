#include "bag.h"
#include "pennant.h"

Bag::Bag(){
  this->backbone_size = 20;
  this->backbone = new Pennant** [this->backbone_size];
  //set our backbone indices to NULL
  for(int i=0; i<this->backbone_size; ++i){
    this->backbone[i] = NULL;
  }
}

void Bag::bag_insert(int vertex){
  Pennant* newPennant = new Pennant(vertex);
  int iter = 0;
  while (this->backbone[iter]!= NULL) {
    newPennant = this->backbone[iter]->pennant_union(newPennant);
    this->backbone[iter++] = NULL;
  }
  this->backbone[iter] = newPennant;
}

void Bag::bag_union(Bag* bag){
  Pennant* y = NULL; //"carry" bit
  for(int i=0;i<this->backbone_size; ++i){
    this->backbone[i] = full_adder(bag->backbone[i],y);
  }
}

Bag* Bag::bag_split(){
  Bag* s2 = new Bag();
  Pennant* y = this->backbone[0];
  this->backbone[0] = NULL;
  for(int i=0; i<this->backbone_size;++i){
    if(this->backbone[i]!=NULL){
      s2->backbone[i-1] = this->backbone[i]->pennant_split();
      this->backbone[i-1] = this->backbone[i];
      this->backbone[i] = NULL;
    }
  }
  if(y->root != NULL)
    this->bag_insert(y);
}
