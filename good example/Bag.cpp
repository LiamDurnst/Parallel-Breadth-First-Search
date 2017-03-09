/*************************************
 *              Bag.cpp              *
 * by Dane Pitkin and Nathan Crandall*
 *        for CS140 Final Project    *
 *      in Parallel BFS Algorithm    *
 *                                   *
 *                                   *
 ************************************/


#include "Bag.h"

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
