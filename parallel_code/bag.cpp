#include "bag.h"
#include "pennant.h"

extern "C++"{

Bag::Bag()
{
  this->forest_size = 20;
  this->forest = (Pennant**)malloc(sizeof(Pennant*)*forest_size);
  for(int i = 0; i < forest_size; i++)
    this->forest[i] = NULL;
}

Bag::Bag(const Bag* & bag)
{
  for(int i = 0; i< forest_size; i++){
    if (bag->forest[i] != NULL)
      this->forest[i] = bag->forest[i];
  }
}

Bag::~Bag()
{
  for(int i = 0; i < this->forest_size; i++)
    //delete this->forest[i];
    this->forest[i] = NULL;
}

void Bag::insert(int item)
{
  int i = 0;
  Pennant* x = new Pennant(item);
  while (this->forest[i] != NULL){
    x = this->forest[i]->Pmerge(x);
    forest[i++] = NULL;
  }
  forest[i] = x;
}

bool Bag::empty() const
{
  for (int i = 0; i < this->forest_size; i++)
    if (this->forest[i] != NULL)
      return false;

  return true;
}

void Bag::merge(Bag* bag)
{
  Pennant* carry = NULL;
  int size = this->forest_size;
  for(int i = 0; i < size; i++){
    if (this->forest[i] != NULL)
      this->forest[i] = this->forest[i]->Pmerge_FA(bag->forest[i], carry);
    else if (bag->forest[i] != NULL)
      this->forest[i] = bag->forest[i]->Pmerge_FA(this->forest[i], carry);
    else if (carry != NULL){
      this->forest[i] = carry;
      carry = NULL;
    }
    else
      this->forest[i] = NULL;

  }

}

Bag* Bag::split()
{
  Bag* new_bag = new Bag();
  Pennant* y = this->forest[0];
  this->forest[0] = NULL;

  for(int i = 1; i < this->forest_size; i++){
    if (this->forest[i] != NULL){
      new_bag->forest[i-1] = this->forest[i]->Psplit();
      this->forest[i-1] = this->forest[i];
      this->forest[i] = NULL;
    }
  }

  if (y != NULL){
    insert(y->root->item);
    y->remove_all(y->root);
    delete y;
    y = NULL;
  }

  return new_bag;
}

void Bag::clear()
{
  //delete [] this->forest;
  for(int i = 0; i < forest_size; i++){
    //delete this->forest[i];
    this->forest[i] = NULL;
  }
}

int Bag::size() const
{
  int sum = 0;
  for(int i = 0; i < forest_size; i++){
    if (forest[i] != NULL)
      sum += (int)pow(2, i);
  }
  return sum;
}

//function not used
bool Bag::can_split() const
{
  return false;
}

void Bag::print() const
{
  int sum = 0;
  printf("***********TREES CONTAINING ITEMS:*************\n");
  for(int i = 0; i < this->forest_size; i++){
    sum = 0;
    if (this->forest[i] != NULL){
      recursive_print_sum(forest[i]->root, sum);
      printf("forest[%d] has tree of size 2^%d = %d\n", i, i, sum);
    }
    //else
      //printf("forest[%d] = NULL\n", i);
  }

  printf("\n\n***********PRINTING CONTENTS:*************\n");
  for(int i = 0; i < forest_size; i++){
    if (forest[i] != NULL){
      printf("forest[%d] contains: \n", i);
      recursive_print(forest[i]->root);
    }
    else
      printf("forest[%d] = NULL\n", i);
  }
  printf("\n***********FINISHED*************\n\n");
}

//prints in-order
void Bag::recursive_print(Node* node) const
{
  if (node->left != NULL)
    recursive_print(node->left);
  printf("%d\n", node->item);
  if (node->right != NULL)
    recursive_print(node->right);
}

void Bag::recursive_print_sum(Node* node, int & sum) const
{
  sum++;
  if (sum == 1){
    if (node->left != NULL)
      recursive_print_sum(node->left, sum);
  }
  else if (node->left != NULL)
    recursive_print_sum(node->left, sum);
  if (node->right != NULL)
    recursive_print_sum(node->right, sum);
}

void Bag::remove_all()
{
  for(int i = 0; i < forest_size; i++){
    this->forest[i]->remove_all(this->forest[i]->root);
    delete this->forest[i];
    this->forest[i] = NULL;
  }
}

int* Bag::write_array()
{
  int size = this->size();
  int* array = new int [size];
  int count = 0;
  for(int i = 0; i < this->forest_size; i++){
    if(this->forest[i] != NULL)
      recursive_write_array(array, this->forest[i]->root, count);
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

}//end extern c++


// // default constructor for bag
// Bag::Bag(){
//   this->backbone_size = 20;
//   this->backbone = new Pennant* [this->backbone_size];
//   //set our backbone indices to NULL
//   for(int i=0; i<this->backbone_size; ++i){
//     this->backbone[i] = NULL;
//   }
// }
//
// Bag::~Bag()
// {
//   for(int i = 0; i < this->backbone_size; i++)
//     //delete this->forest[i];
//     this->backbone[i] = NULL;
// }
//
// // insert one vertex into bag
// void Bag::bag_insert(int vertex){
//   Pennant* newPennant = new Pennant(vertex);
//   int iter = 0;
//   // search for first open index
//   while (this->backbone[iter] != NULL) {
//     newPennant = this->backbone[iter]->pennant_union(newPennant);
//     this->backbone[iter++] = NULL;
//   }
//   this->backbone[iter] = newPennant;
//   return;
// }
//
// // merge 'this' with 'bag'
// void Bag::bag_union(Bag* bag){
//   Pennant* y = NULL;
//   for(int i = 0; i < this->backbone_size; i++){
//     if (this->backbone[i] != NULL)
//       this->backbone[i] = this->backbone[i]->full_adder(bag->backbone[i], y);
//     else if (bag->backbone[i] != NULL)
//       this->backbone[i] = bag->backbone[i]->full_adder(this->backbone[i], y);
//     else if (y != NULL){
//       this->backbone[i] = y;
//       y = NULL;
//     } else {
//       this->backbone[i] = NULL;
//     }
//   }
//   return;
// }
//
// // check if bag is empty
// bool Bag::is_empty(){
//   for(int i=0; i<this->backbone_size; i++) {
//     if(this->backbone[i]!=NULL)
//       return false;
//   }
//   return true;
// }
//
// // set all bag indices to NULL
// void Bag::reset(){
//   for(int i=0; i<this->backbone_size; i++){
//     this->backbone[i] = NULL;
//   }
// }
//
// // count number of vertices in bag
// int Bag::n_vertices(){
//   int result = 0;
//   for(int i=0; i<this->backbone_size; i++){
//     if(this->backbone[i]!=NULL)
//       result += pow(2,i);
//   }
//   return result;
// }
//
// // split bag using arithmetic right shift
// Bag* Bag::bag_split(){
//   Bag* s2 = new Bag();
//   Pennant* y = this->backbone[0];
//   this->backbone[0] = NULL;
//   for(int i=1; i<this->backbone_size; i++){
//     if(this->backbone[i]!=NULL){
//       s2->backbone[i-1] = this->backbone[i]->pennant_split();
//       this->backbone[i-1] = this->backbone[i];
//       this->backbone[i] = NULL;
//     }
//   }
//   if(y != NULL){
//     this->bag_insert(y->root->vertex);
//     y->remove_all(y->root);
//     delete y;
//     y = NULL;
//   }
//   return s2;
// }
