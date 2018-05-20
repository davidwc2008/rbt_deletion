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
  bool done = false;
  Node* current = getNode(value);

  while (!done) {
    //root case (current->parent == NULL)
    if (current == root) {
      current->color = BLACK;
      done = true;
      //cout << "Ran root case" << endl;
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
	  //cout << "right right" << endl;
	}
	//SUBSubcase #4: right left case
	else {
	  rotateRight(parent);
	  current = parent;
	  //repeat right right case
	  //cout << "right left" << endl;
	}
      }
    } // the end of the if
    else if (current->parent->color == BLACK) {
      done = true;
      //cout << "parent is black" << endl;
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
    while (counter != levelsize) {
      Node* current = queue.front();
      queue.pop();
      counter++;
      if (current == NULL || current->isNULL()) {
        cout << "-,";
        queue.push(NULL);
        queue.push(NULL);
      } else {
        cout << current->value << (current->color==BLACK?"B":"R") << ",";
        moredata = true;
        queue.push(current->left);
        queue.push(current->right);
      }
    }
    cout << endl;
  }
}

void RBTree::BSTadd(Node *& root, int value) {
  if (root->isNULL()) {
    root->value = value;
    root->color = RED;
    root->setLeft(new Node());
    root->setRight(new Node());
  } else if (root->value > value) {
    BSTadd(root->left, value);
  } else if (root->value < value) {
    BSTadd(root->right, value);
  }
}

//standard BST remove with some changes
void RBTree::remove(Node *& root, int value) {
  Node* u; //the node that replaces v
  Node* v = getNode(value); // the deleted node

  //Step 1: Standard BST remove with u and v node pointers
  if (search(value) == true) {
    if (value < root->value) {
      remove(root->left, value);
    } else if (value > root->value) {
      remove(root->right, value);
    } else {
      //leaf case
      if ((root->right->isNULL()) && (root->left->isNULL())) {
	delete root;
	root->setNULL();
	u = root;
      }
      //internal node with only left child
      else if ((root->right->isNULL())) {
	Node* todelete = root;
	root = root->left;
	delete todelete;
	u = root;
      }
      //internal node with only right child
      else if ((root->left->isNULL())) {
        Node* todelete = root;
        root = root->right;
        delete todelete;
	u = root;
      }
      //internal node has two children
      else {
	//find in order successor
	Node* successor = root->right;
	while (!(successor->left->isNULL())) {
	  successor = successor->left;
	}
	u = successor;
	root->value = successor->value;
	remove(root->right, successor->value);
      }
    }
  } else {
    cout << "Cannot remove.  The number is not present." << endl;
  }
  //Step 2: Simple Case (if either u or v is red)
  if ((u->color == RED) || (v->color == RED)) {
    u->color = BLACK;
  }
  //Step 3: If both u and v are black
  else if ((u->color == BLACK) && (v->color == BLACK)) {
    //step 3,1
    u->color = DOUBLE_BLACK;
    //step 3.2
    if ((u->color == DOUBLE_BLACK) && (u != root)) {
      Node* s = u->getSibling();
      //step 3.2a
      if ((s->color == BLACK) && ((s->left->color == RED) || (s->right->color == RED))) {
	if (s->left->color == RED) {
	  Node* r = s->left;
	} else {
	  Node* r = s->right;
	}
	//step 3.2a(i)

	//step 3.2a(ii)

	//step 3.2a(iii)

	//step 3.2a(iv)
	
      }
      //step 3.2b
      else if ((s->color == BLACK) && ((s->left->color == BLACK) && (s->right->color == BLACK))) {

      }
      //step 3.2c
      else if (s->color == RED) {
	//step 3.2c(i)
	//step 3.2c(iii)
      }
      //step 3,3
      else if (u == root) {
	u->color = BLACK;
      }
    } //step 3.2
  } //step 3
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

void RBTree::rotateLeft(Node* node) {
  Node* parent = node->parent;
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
    root == node->left;
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
