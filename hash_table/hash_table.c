#include "list.h"
#include "hash_table.h"

int hash_function(const hashtable* ht,int page){
  return (page % ht->buckets);
}


void ht_initialize(hashtable* ht,int buckets) {
  ht->table=malloc(sizeof(hashtable_node)*buckets);
  ht->buckets=buckets;
  for (int i=0;i<buckets;i++){
    ht->table[i].key=i;
    ht->table[i].head=NULL;
  }
  ht->numberOfNodes=0;
}


int ht_search(const hashtable* ht, int page,int number) {
  // searching based on page number
  int index=hash_function(ht, page);
  list_node* ln=list_search_page_num(ht->table[index].head,page);
  if (ln != NULL){  // if found page in the hashed page table, then return the corresponding frame number
    printf("\n>Found Page: %d in %d hashtable\n",page,number);
    return ln->n.frame_num;
  }
  else {
    printf(">Page: %d not found in %d hashtable\n",page,number);
    return -1;
  }
}


void ht_insert(hashtable* ht, int page, int frame){
  // insert/load a page/node in to the hashed page table
  int index=hash_function(ht,page);
  printf(">Inserting page in to the hashtable, bucket number: %d\n",index);
  ht->table[index].head=list_insert(ht->table[index].head,page,frame);
  ht->numberOfNodes++;
}


void ht_print(const hashtable* ht){
  for (int i=0;i<ht->buckets;i++){
    printf("\nBucket %d: ",i+1);
    list_print(ht->table[i].head);
  }
  printf("\n\n");
}


void ht_delete(hashtable* ht){
  // deleting whole hashed page table
  for (int i=0;i<ht->buckets;i++){
    ht->table[i].head=list_delete(ht->table[i].head);
  }
  free(ht->table);
  printf(">>Hashtable Deleted\n");
}


int ht_delete_node(hashtable* ht,int frame){
  // delete the hashed page table's page/node with the corresponding frame number
  node n;
  for (int i=0;i<ht->buckets;i++){
    list_node* ln=list_search_frame_num(ht->table[i].head,frame);
    if (ln==NULL) continue; // if not found at current bucket, then continue
    n=ln->n;
    if (n.frame_num==frame){
      printf(">Deleting entry with Page Number:%d and Frame Number:%d\n",n.page_num,n.frame_num);
      ht->table[i].head=list_delete_node(ht->table[i].head,n);
      ht->numberOfNodes--;
      return 1;
    }
  }
  return 0;
}
