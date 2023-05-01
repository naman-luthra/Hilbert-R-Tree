#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct LLNode {
    void * data;
    struct LLNode * next;
    struct LLNode * prev;
} LLNode;

typedef struct LinkedList {
    LLNode *head;
    LLNode *tail;
    int count;
} LinkedList;

/*
    * Function: createLinkedList
    * -------------------------------
    *  Creates a new linked list
    *  Time complexity: O(1)
*/
LinkedList * createLinkedList();

/*
    * Function: freeLinkedList
    * -------------------------------
    *  Frees the linked list
    *  Time complexity: O(n)
    *  n is number of nodes in the linked list
*/
void freeLinkedList(LinkedList * list);

/*
    * Function: llInsert
    * -------------------------------
    *  Inserts a new node into the linked list
    *  ll: linked list to be inserted into
    *  data: data to be inserted
    *  Time complexity: O(1)
*/
void llInsert(LinkedList * list, void * data);

/*
    * Function: llDelete
    * -------------------------------
    *  Deletes a node from the linked list
    *  ll: linked list to be deleted from
    *  node: node to be deleted
    *  Time complexity: O(1)
*/
void llDelete(LinkedList * ll, LLNode * node);

#endif