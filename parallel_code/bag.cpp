#include "bag.h"
#include "pennant.h"

// default constructor for bag
Bag::Bag(){
  this->backbone_size = 20;
  this->backbone = new Pennant* [this->backbone_size];
  //set our backbone indices to NULL
  for(int i=0; i<this->backbone_size; ++i){
    this->backbone[i] = NULL;
  }
}

// insert one vertex into bag
void Bag::bag_insert(int vertex){
  Pennant* newPennant = new Pennant(vertex);
  int iter = 0;
  // search for first open index
  while (this->backbone[iter] != NULL) {
    newPennant = this->backbone[iter]->pennant_union(newPennant);
    this->backbone[iter++] = NULL;
  }
  this->backbone[iter] = newPennant;
  return;
}

// merge 'this' with 'bag'
void Bag::bag_union(Bag* bag){
  Pennant* y = NULL;
  for(int i = 0; i < this->backbone_size; i++){
    if (this->backbone[i] != NULL)
      this->backbone[i] = this->backbone[i]->full_adder(bag->backbone[i], y);
    else if (bag->backbone[i] != NULL)
      this->backbone[i] = bag->backbone[i]->full_adder(this->backbone[i], y);
    else if (y != NULL){
      this->backbone[i] = y;
      y = NULL;
    } else {
      this->backbone[i] = NULL;
    }
  }
  return;
}

// check if bag is empty
bool Bag::is_empty(){
  for(int i=0; i<this->backbone_size; i++) {
    if(this->backbone[i]!=NULL)
      return false;
  }
  return true;
}

// set all bag indices to NULL
void Bag::reset(){
  for(int i=0; i<this->backbone_size; i++){
    this->backbone[i] = NULL;
  }
}

// count number of vertices in bag
int Bag::n_vertices(){
  int result = 0;
  for(int i=0; i<this->backbone_size; i++){
    if(this->backbone[i]!=NULL)
      result += pow(2,i);
  }
  return result;
}

// split bag using arithmetic right shift
Bag* Bag::bag_split(){
  Bag* s2 = new Bag();
  Pennant* y = this->backbone[0];
  this->backbone[0] = NULL;
  for(int i=1; i<this->backbone_size; i++){
    if(this->backbone[i]!=NULL){
      s2->backbone[i-1] = this->backbone[i]->pennant_split();
      this->backbone[i-1] = this->backbone[i];
      this->backbone[i] = NULL;
    }
  }
  if(y != NULL){
    this->bag_insert(y->root->vertex);
    y->remove_all(y->root);
    delete y;
    y = NULL;
  }
  return s2;
}

int* Bag::write_array()
{
  int size = this->n_vertices();
  int* array = new int [size];
  int count = 0;
  for(int i = 0; i < this->backbone_size; i++){
    if(this->backbone[i] != NULL)
      recursive_write_array(array, this->backbone[i]->root, count);
  }

  return array;
}

void Bag::recursive_write_array(int* & array, Node* node, int &count)
{
  if (node->left != NULL)
    recursive_write_array(array, node->left, count);
  array[count++] = node->item;
  if (node->right != NULL)
    recursive_write_array(array, node->right, count);
}
