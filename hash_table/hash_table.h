#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct hashtable_node hashtable_node;
typedef struct hashtable hashtable;

struct hashtable_node{
  int key;
  list_node* head;
};

struct hashtable{
  hashtable_node* table;
  int buckets;
  int numberOfNodes;
};

int hash_function(const hashtable*,int);
void ht_initialize(hashtable *,int);
int ht_search(const hashtable*,int,int);
void ht_insert(hashtable*,int,int);
void ht_print(const hashtable*);
void ht_delete(hashtable*);
int ht_delete_node(hashtable*,int);
#endif
