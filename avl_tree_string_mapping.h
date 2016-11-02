#ifndef _AVL_TREE_STRING_MAPPING_
  
#define _AVL_TREE_STRING_MAPPING_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "types.h"


typedef struct  Avl_node {  
   char name[NAME_SIZE]; // original name of the node
   int mapping;  // the mapping assigned to the node
   struct Avl_node *left,*right;  // tree left and right pointers
   int ht; // height information for the avl tree
} Avl_node;

Avl_node *root_node;  // the root of the avl tree
unsigned int current_mapping; //the latest mapping used, either new or old

unsigned int ground_found;  //flag

/*This function maps the name string to a unique integer identifier.
Checks whether the same name already exists (making non case-sensitive comparison) otherwise creates
a new mapping.Returns the integer identifier.*/
int insert_node(char *x);
Avl_node * insert(Avl_node *, char *);
Avl_node *Delete(Avl_node *, char *);
int search_for_node(char *x);
int search_for_node_mapping(Avl_node *T, char *x);
void free_avl_tree(void);
void free_tree_space(Avl_node *T);
void  preorder(Avl_node *);
void  inorder(Avl_node *);
int   height(Avl_node *);
Avl_node *rotateright(Avl_node *);
Avl_node *rotateleft(Avl_node *);
Avl_node *RR(Avl_node *);
Avl_node *LL(Avl_node *);
Avl_node *LR(Avl_node *);
Avl_node *RL(Avl_node *);
int BF(Avl_node *);

#endif
