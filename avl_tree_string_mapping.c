#include "avl_tree_string_mapping.h"


int insert_node(char *x) {
  int i;
  
    /*check whether the node is the ground */
    if(!strcmp(x, "0")) {
        ground_found = 1;
        return 0;
    }
      
    for(i = 0; x[i] != '\0'; i++){ //Convert to lower Case
        x[i] = tolower(x[i]);
    }
      
    root_node = insert(root_node, x);
    
    return current_mapping;
  
}

inline Avl_node * insert(Avl_node *T, char *x) {
  
    if(T == NULL) { //new node
        T = (Avl_node*)malloc(sizeof(Avl_node));
	
	if(T == NULL) {
	    printf("Could not allocate new node.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	
	strcpy(T->name, x);
	current_mapping = T->mapping = ++number_of_nodes;
        T->left=NULL;
        T->right=NULL;
	
	#if DEBUG
	printf("Node Mapping, %s -> %d.\n", T -> name, T -> mapping);
        #endif
	
	
	circuit_simulation.plot_settings = (plotSettings *) realloc(circuit_simulation.plot_settings, (number_of_nodes + 1) * sizeof(plotSettings));
    
	if(circuit_simulation.plot_settings == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	
	strcpy((circuit_simulation.plot_settings + number_of_nodes) -> name, T -> name);
	(circuit_simulation.plot_settings + number_of_nodes) -> dc_sweep_plot = 0;
	(circuit_simulation.plot_settings + number_of_nodes) -> transient_plot = 0;

    }
    else {
        if(strcasecmp(x, T->name) > 0)	 {// insert in right subtree
            T->right=insert(T->right, x);
            if(BF(T)==-2) {
		if(strcasecmp(x, T->right->name) > 0) {
                    T=RR(T);
		}
                else {
                    T=RL(T);
		}
	    }
        }
        else {
	    if(strcasecmp(x, T->name) < 0) { // insert in left subtree
                T->left=insert(T->left, x);
                if(BF(T)==2) {
		    if(strcasecmp(x, T->left->name) < 0) {
                        T=LL(T);
		    }
                    else {
                        T=LR(T);
		    }
		}
            }
            else { //already in the tree
	        current_mapping = T-> mapping;
	    }
	}
      
    }
    
    T->ht=height(T);
    
    return(T);
    
}

int search_for_node(char *x) {
  int i;
    
     if(!strcmp(x, "0") && ground_found == 0) { // requested ground for plotting, but ground was not found yet
        return -1;
     }
     else if(!strcmp(x, "0") && ground_found == 1) { // ground was found, return its mapping
        return 0; 
     }
     
     for(i = 0; x[i] != '\0'; i++){ //Convert to lower Case
        x[i] = tolower(x[i]);
     }
     
     return search_for_node_mapping(root_node, x);
    
}


inline int search_for_node_mapping(Avl_node *T, char *x) {

    if(T == NULL) {
        return -1;
    }
    else {
	if(strcmp(x, T->name) > 0) {
            return search_for_node_mapping(T->right, x);
        }
        else if(strcmp(x, T->name) < 0) {
            return search_for_node_mapping(T->left, x);
        }
        else {
            return T -> mapping;
	}
    }

}


void free_avl_tree(void) {
  
    free_tree_space(root_node);
  
}

inline void free_tree_space(Avl_node *T) {
  
    if(T != NULL) {
        free_tree_space(T->left);
        free_tree_space(T->right);
	free(T);
    }

}


inline Avl_node * Delete(Avl_node *T, char *x)
{       Avl_node *p;

    if(T==NULL)
    {
        return NULL;
    }
    else

	if(strcmp(x, T->name) > 0)
        {
            T->right=Delete(T->right, x);
            if(BF(T)==2)
                if(BF(T->left)>=0)
                    T=LL(T);
                else
                    T=LR(T);
        }
        else
	    if(strcmp(x, T->name) < 0)
                {
                    T->left=Delete(T->left,x);
                    if(BF(T)==-2)//Rebalance during windup
                        if(BF(T->right)<=0)
                            T=RR(T);
                        else
                            T=RL(T);
                }
            else
              {
                //data to be deleted is found
                  if(T->right !=NULL)
                  {  //delete its inorder succesor
                      p=T->right;
                      while(p->left != NULL)
                      p=p->left;

		      strcpy(T->name, p->name);
                      T->right=Delete(T->right,p->name);
                      if(BF(T)==2)//Rebalance during windup
                        if(BF(T->left)>=0)
                            T=LL(T);
                        else
                            T=LR(T);
                   }
                  else
                   return(T->left);

              }
    T->ht=height(T);
    return(T);
}

inline int height(Avl_node *T)
{
    int lh,rh;
    if(T==NULL)
        return(0);
    if(T->left==NULL)
        lh=0;
    else
        lh=1+T->left->ht;
    if(T->right==NULL)
        rh=0;
    else
        rh=1+T->right->ht;
    if(lh>rh)
        return(lh);
    return(rh);
}

inline Avl_node * rotateright(Avl_node *x)
{
    Avl_node *y;
    y=x->left;
    x->left=y->right;
    y->right=x;
    x->ht=height(x);
    y->ht=height(y);
    return(y);
}

inline Avl_node * rotateleft(Avl_node *x)
{
    Avl_node *y;
    y=x->right;
    x->right=y->left;
    y->left=x;
    x->ht=height(x);
    y->ht=height(y);
    return(y);
}

inline Avl_node * RR(Avl_node *T)
{
    T=rotateleft(T);
    return(T);
}

inline Avl_node * LL(Avl_node *T)
{
    T=rotateright(T);
    return(T);
}

inline Avl_node * LR(Avl_node *T)
{
    T->left=rotateleft(T->left);
    T=rotateright(T);
    return(T);
}

inline Avl_node * RL(Avl_node *T)
{
    T->right=rotateright(T->right);
    T=rotateleft(T);
    return(T);
}

inline int BF(Avl_node *T)
{
    int lh,rh;
    if(T==NULL)
    return(0);
    if(T->left==NULL)
        lh=0;
    else
        lh=1+T->left->ht;
    if(T->right==NULL)
        rh=0;
    else
        rh=1+T->right->ht;
    return(lh-rh);
}

void preorder(Avl_node *T)
{
    if(T!=NULL)
    {
        printf(" %s(Bf=%d)",T->name,BF(T));
        preorder(T->left);
        preorder(T->right);
    }
}
void inorder(Avl_node *T)
{
    if(T!=NULL)
    {
        inorder(T->left);
        printf(" %s(Bf=%d)",T->name,BF(T));
        inorder(T->right);
    }
}
