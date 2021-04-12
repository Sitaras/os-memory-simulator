#include "list.h"

list_node* allocate_list_node(){
  list_node* node=malloc(sizeof(list_node));
  return node;
}

list_node* list_insert(list_node* list,int page,int frame){
  if (list==NULL){
    // list is empty
    list_node* node=allocate_list_node();
    node->n.page_num=page;
    node->n.frame_num=frame;
    node->next=NULL;
    list=node;
    return list;
  }
  // insert a new node at the end of the list
  list_node* temp = list;
  while (temp->next!=NULL){
        temp = temp->next;
  }
  list_node* node=allocate_list_node();
  node->n.page_num=page;
  node->n.frame_num=frame;
  node->next=NULL;
  temp->next=node;
  return list;
}


list_node* list_delete_node(list_node *list, node n){
   list_node *temp=list, *prev;
   if (temp!=NULL && temp->n.page_num==n.page_num && temp->n.frame_num==n.frame_num ){
       // list has only one node
       list=temp->next;
       free(temp);  // delete node
       return list;
   }
   while (temp!=NULL && temp->n.page_num!=n.page_num && temp->n.frame_num!=n.frame_num){
       prev=temp;
       temp=temp->next;
   }

   if (temp==NULL) return list; // not found

   prev->next=temp->next; // unlink the node from the list
   free(temp);  // and delete the node
   return list;
}


list_node* list_search_page_num(list_node *list, int page){
  // searching based on page number
  list_node* current=list;
  while (current!=NULL){
      if (current->n.page_num==page)
          return current;
      current=current->next;
  }
  return NULL;
}


list_node* list_search_frame_num(list_node *list, int frame){
  // searching based on frame number
  list_node* current = list;
  while (current!=NULL){
      if (current->n.frame_num==frame)
          return current;
      current=current->next;
  }
  return NULL;
}


void list_print(list_node *list){
    if(list==NULL) return;
    list_node *temp=list;
    while(temp!=NULL){
        printf(" --P:%d-F:%d-- ",temp->n.page_num,temp->n.frame_num);
        temp=temp->next;
    }
}


list_node* list_delete(list_node *list){
    // deleting whole list
    if(list==NULL) return NULL;

    list_node *current = list;
    list_node *next;

    while(current!=NULL){
        next=current->next;
        free(current);
        current=next;
    }
    list=NULL;
    return list;
}
