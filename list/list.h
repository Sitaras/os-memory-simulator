#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct node node;
typedef struct list_node list_node;

struct node{
  int page_num;
  int frame_num;
};

struct list_node {
  node n;
  list_node* next;
};

list_node* allocate_list_node();
list_node* list_insert(list_node *, int,int);
list_node* list_search_page_num(list_node * ,int );
list_node* list_search_frame_num(list_node *, int);
void list_print(list_node *);
list_node* list_delete(list_node *);
list_node* list_delete_node(list_node *, node);

#endif
