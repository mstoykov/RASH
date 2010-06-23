#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdlib.h>

struct node {
    char * c;
    struct node * next;
}  ;

typedef struct {
    struct node * first;
    struct node * last;
    int size;
} list;

void init (list ** );

void addToList (list * , char * );

void removeFirst(list * );

void freeAll(list * );
#endif
