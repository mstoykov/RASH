#include "linked_list.h"

void init (list ** list){
    *list = malloc(sizeof(**list));
    (*list)->first = NULL;
    (*list)->last = NULL;
    (*list)->size = 0;
}
void addToList (list * list_, char * c){
    struct node  * l;
    l =  malloc(sizeof *l);
    l->c = c;
    l->next = NULL;
    if (list_->last == NULL){
        list_->last = l;
        list_->first= l;
    }
    list_->last->next = l;
    list_->last = l;
    list_->size++;
}

void removeFirst(list * list_){
    if( list_->size ==0) return; //empty
    struct node * l = list_->first;
    list_->first = list_->first->next;
    if (list_->first == NULL)
        list_->last = NULL;
    free(l->c);
    free(l);
    list_->size--;
}

void freeAll(list * list_){
    while(list_->size != 0)
        removeFirst(list_);
}
