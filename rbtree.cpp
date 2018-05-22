#include "rbtree.h"
#include <queue>

using namespace std;

Node::Node(int value) {
  this->value = value;
  this->color = RED;
  parent = right = left = NULL;
}

//default "empty" NULL node
Node::Node() {
  this->setNULL();
  parent = left = right = NULL;
}

//check if we have a NULL node
bool Node::isNULL() {
  return this->value == 0;
}

//zero node indicates that it is NULL
void Node::setNULL() {
  this->value = 0;
  this->color = BLACK; //by definition
}

Node* Node::getSibling() {
  if (this->parent == NULL) {
    return NULL;
  }
  if (this->isLeft()) {
    return this->parent->right;
  } else {
    return this->parent->left;
  }
}

Node* Node::getGrandparent() {
  if (this->parent == NULL) {
    return NULL;
  }
  return this->parent->parent;
}

Node* Node::getUncle() {
  if (this->parent == NULL) {
    return NULL;
  } else {
    return this->parent->getSibling();
  }
}

//set right child
void Node::setRight(Node* child) {
  this->right = child;
  child->parent = this;
}

//set left child
void Node::setLeft(Node* child) {
  this->left = child;
  child->parent = this;
}

//check to see if it is a left child
bool Node::isLeft() {
  if (this->parent) {
    return this->parent->left == this;
  }
  return false;
}

//check to see if it is a right child
bool Node::isRight() {
  if (this->parent) { //it exists
    return this->parent->right == this;
  }
  return false;
}

//tree constructor
RBTree::RBTree() {
  root = new Node();
}

void RBTree::add(int value) {
  BSTadd(root, value); // this is step one
  cout << "Root value: " << root->value << endl;
  bool done = false;
  Node* current = getNode(value);
  cout << "Made it to add" << endl;
  cout << "Current value: " << current->value << endl;
  while (!done) {
    //root case (current->parent == NULL)
    if (current == root) {
      current->color = BLACK;
      done = true;
      cout << "Ran root case" << endl;
    }
    //x parent is not black case
    else if (current->parent->color != BLACK) {
      Node* parent = current->parent;
      Node* uncle = current->getUncle();
      Node* grandparent = current->getGrandparent();
      //Subcase #1: Uncle is red
      if (uncle->color == RED) {
	parent->color = BLACK;
	uncle->color = BLACK;
	grandparent->color = RED;
	current = grandparent; //while loop repeats
	//cout << "Uncle is red" << endl;
      } else {
	//Subcase #2: Uncle is black
	//SUBSubcase #1: left left
	if (parent->isLeft() && current->isLeft()) {
	  rotateRight(grandparent);
	  int temp = parent->color;
	  parent->color = grandparent->color;
	  grandparent->color = temp;
	  done = true;
	  //cout << "Left left" << endl;
	}
	//SUBSubcase #2: left right
	else if (parent->isLeft() && current->isRight()) {
	  rotateLeft(parent);
	  current = parent;
	  //repeat left left case
	  //cout << "Left right" << endl;
	}
	//SUBSubcase #3: right right case
	else if (parent->isRight() && current->isRight()) {
	  rotateLeft(grandparent);
	  int temp = parent->color;
          parent->color = grandparent->color;
          grandparent->color = temp;
          done = true;
	  cout << "right right" << endl;
	}
	//SUBSubcase #4: right left case
	else {
	  rotateRight(parent);
	  current = parent;
	  //repeat right right case
	  cout << "right left" << endl;
	}
      }
    } // the end of the if
    else if (current->parent->color == BLACK) {
      done = true;
      cout << "parent is black" << endl;
    }
  } // while loop
} // the function

//same as BST print
void RBTree::print() {
  queue<Node*> queue; //creates a queue to print the tree (FIFO data structure)
  queue.push(root);
  bool moredata = true;
  while (moredata) {
    moredata = false;
    int levelsize = queue.size();
    int counter = 0;
    cout << "Level size: " << levelsize << endl;
    while (counter != levelsize) {
      Node* current = queue.front();
      cout << "Current's value: " << current->value << endl;
      queue.pop();
      counter++;
      if (current == NULL || current->isNULL()) {
        cout << "-" << (current->color==BLACK?"B":"R") << ",";
        queue.push(NULL);
        queue.push(NULL);
	cout << "In the if" << endl;
      } else {
	cout << "In the else" << endl;
        cout << current->value << (current->color==BLACK?"B":"R") << ",";
        moredata = true;
        queue.push(current->left);
        queue.push(current->right);
      }
    }
    cout << endl;
  }
  cout << "Print ended" << endl;
}

void RBTree::BSTadd(Node *& root, int value) {
  if (root->isNULL()) {
    root->value = value;
    root->color = RED; //make the newly inserted node as RED
    root->setLeft(new Node());
    root->setRight(new Node());
    cout << "Root value in BST: " << root->value << endl;
  } else if (root->value > value) {
    BSTadd(root->left, value);
  } else if (root->value < value) {
    BSTadd(root->right, value);
  }
  cout << "Root value at the end of BST: " << root->value << endl;
}

//standard BST remove with changes
void RBTree::remove(int value) {
  Node* u; //the node that replaces v
  Node* v = getNode(value); // the deleted node
  cout << "Color of v at first: " << v->color << endl;
  cout << "Color of v child" << v->left->color << endl;
  //Step 1: Standard BST remove with u and v node pointers
  if (searchNode(value)) {

    //set v
    if (!(v->left->isNULL()) && !(v->right->isNULL())) {
      //find in order successor
      Node* successor = v->right;
      while (!(successor->left->isNULL())) {
        successor = successor->left;
      }
      v->value = successor->value;
      v = successor;
      cout << "v set to successor" << endl;
    }
    cout << "Value of v: " << v->value << endl;
    cout << "Color of v at first: " << v->color << endl;
    //set u
    //leaf case
    if (v->right->isNULL() && v->left->isNULL()) {
      cout << "Color of v at 0.20: " << v->color << endl;
      u = v->left;
      delete v->right;
      cout << "LEAF!!!" << endl;
    }
    //internal node with only left child
    else if (v->right->isNULL()) {
      u = v->left;
      delete v->right;
    }
    //internal node with only right child
    else if (v->left->isNULL()) {
      u = v->right;
      delete v->left;
    }
    cout << "Value of u: " << u->value << endl;
    cout << "Color of v at 0.5 time" << v->color << endl;
    //remove v
    //root case first
    if (v == root) {
      u->parent = NULL;
      root = u;
    } else {
      if (v->isLeft()) {
	v->parent->setLeft(u);
      } else {
	v->parent->setRight(u);
      }
    }
    cout << "Value of v 2nd time: " << v->value << endl;
    cout << "Color of v: " << v->color << endl;
    cout << "Color of u: " << u->color << endl;
    
    //rebalancing the tree
    //Step 2: Simple case
    if (u->color == RED || v->color == RED) {
      u->color = BLACK;
      cout << "Simple case" << endl;
    }
    //Step 3: If both u and v are black
    else if (u->color == BLACK && v->color == BLACK) {
      //step 3.1
      u->color = DOUBLE_BLACK;
      //step 3.2 and 3.3
      while (u->color == DOUBLE_BLACK) {
	Node* s = u->getSibling();
	cout << "Value of s: " << s->value << endl;
	//step 3.2a
	if (s->color == BLACK && (s->left->color == RED || s->right->color == RED)) {
	  //set r (the red child of sibling)
	  Node* r;
	  if (s->left->color == RED) {
	    r = s->left;
	  } else {
	    r = s->right; //default to right if both are red
	  }
	  cout << "Value of r: " << r->value << endl;
	  //step 3.2a(i) - Left left
	  if (s->isLeft() && r->isLeft()) {
	    if (s->parent->color == BLACK) {
	      r->color = BLACK;
	      rotateRight(s->parent);
	      u->color = BLACK;
	    } else {
	      rotateRight(s->parent);
	      u->parent->color = BLACK;
	      s->color = RED;
	      r->color = BLACK;
	      u->color = BLACK;
	    }
	  } 
	  
	  //step 3.2a(ii) - Left Right
	  else if (s->isLeft() && r->isRight()) {
	    rotateLeft(s);
	    s->color = RED;
	    r->color = BLACK;
	  }
	  //step 3.2a(iii) - Right Right
	  else if (s->isRight() && r->isRight()) {
	    if (s->parent->color == BLACK) {
	      r->color = BLACK;
	      rotateLeft(s->parent);
	      u->color = BLACK;
	    } else {
	      rotateLeft(s->parent);
	      u->parent->color = BLACK;
	      s->color = RED;
	      r->color = BLACK;
	      u->color = BLACK;
	    }
	  }
	  //step 3.2a(iv) - Right left
	  else if (s->isRight() && r->isLeft()) {
	    rotateRight(s);
	    s->color = RED;
	    r->color = BLACK;
	  }
	} //step 3.2a end
      //step 3.2b
	else if (s->color == BLACK && (s->left->color == BLACK && s->right->color == BLACK)) {
	  s->color = RED;
	  cout << "Value of s: " << s->value << endl;
	  s->parent->color++; //red goes to black, black goes to double black
	  u->color = BLACK;
	  u = u->parent;
      }
      
	//step 3.2c
        else if (s->color == RED) {
	  s->color = BLACK;
	  s->parent->color = RED;
	  //step 3.2c(i) - Left
          if (s->isLeft()) {
	    rotateRight(s->parent);
          }
          //step 3.2c(ii) - Right
          else if (s->isRight()) {
            rotateLeft(s->parent);
          }
        } // 3.2c end
	//step 3,3
	else if (u == root) {
	  u->color = BLACK;
	} //step 3.3 end
      } //step 3.2 + 3.3 (while loop)
    } //step 3
    delete v;
  }

  else {
    cout << "Cannot remove.  The number is not present." << endl;
  }
  


} // remove

Node* RBTree::getNode(int value) {
  Node* current = root;
  while (current != NULL) {
    if (current->value == value) {
      return current;
    } else if (current->value > value) {
      current = current->left; 
    } else {
      current = current->right;
    }
  }
  return NULL;
}

void RBTree::search(int value) {
  if (getNode(value) != NULL) {
    cout << "Success: The number is in the tree." << endl;
  } else {
    cout << "Failure: The number is not in the tree." << endl;
  }
}

bool RBTree::searchNode(int value) {
  return (getNode(value) != NULL);
}

void RBTree::rotateLeft(Node* node) {
  Node* parent = node->parent;
  //cout << "Node Parent: " << parent->value << endl;
  if (node == root) {
    root = node->right;
  } else {
    if (node->isLeft()) {
      parent->setLeft(node->right);
    } else {
      parent->setRight(node->right);
    }
  }
  Node* flipped = node->right;
  node->setRight(flipped->left);
  flipped->setLeft(node);  
}

void RBTree::rotateRight(Node* node) {
  Node* parent = node->parent;
  if (node == root) {
    root = node->left;
  } else {
    if (node->isLeft()) {
      parent->setLeft(node->left);
    } else {
      parent->setRight(node->left);
    }
  }
  Node* flipped = node->left;
  node->setLeft(flipped->right);
  flipped->setRight(node);
}
