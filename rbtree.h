//preprocessor code
#ifndef RBTREE_H
#define RBTREE_H

#include <string>
#include <iostream>

//we can define black and red as integers so that it is easier to process and read
#define BLACK 0
#define RED 1

using namespace std;

//declaration of the node struct
struct Node {
  //the basics
  int value;
  int color;
  Node *parent, *right, *left;
  
  //the functions
  Node(int value);
  Node();
  bool isNULL();
  void setNULL();
  Node* getSibling();
  Node* getGrandparent();
  Node* getUncle();
  int getColor();
  void setRight(Node* child);
  void setLeft(Node* child);
  bool isLeft();
  bool isRight();  
};

//declaration of the rbtree class
class RBTree {
 private:
  //basics
  Node* root = NULL;

  //functions
  void BSTadd(Node *&, int value);
  Node* getNode(int value);
  Node* getParent(int value);
  Node* getGrandparent(int value);
  Node* getUncle(int value);
  Node* getSibling(int value);
  void rotateRight(Node* node);
  void rotateLeft(Node* node);
 public:
  RBTree();
  void add(int value);
  void print(); //to print
  void search(int value); //to search
  void remove(int value); //to remove
};

#endif

