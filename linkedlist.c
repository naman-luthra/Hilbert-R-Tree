#include "linkedlist.h"

/*
    * Function: createLinkedList
    * -------------------------------
    *  Creates a new linked list
    *  Time complexity: O(1)
*/
LinkedList * createLinkedList(){
    LinkedList * newList = (LinkedList*) malloc(sizeof(LinkedList));
    newList->head = NULL;
    newList->tail = NULL;
    newList->count = 0;
    return newList;
}

/*
    * Function: llInsert
    * -------------------------------
    *  Inserts a new node into the linked list
    *  ll: linked list to be inserted into
    *  data: data to be inserted
    *  Time complexity: O(1)
*/
void llInsert(LinkedList * ll, void * data){
    LLNode * newNode = (LLNode*) malloc(sizeof(LLNode));
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = ll->tail;
    if(ll->head==NULL){
        ll->head = newNode;
        ll->tail = newNode;
    }
    else{
        ll->tail->next = newNode;
        ll->tail = newNode;
    }
    ll->count++;
}

/*
    * Function: llDelete
    * -------------------------------
    *  Deletes a node from the linked list
    *  ll: linked list to be deleted from
    *  node: node to be deleted
    *  Time complexity: O(1)
*/
void llDelete(LinkedList * ll, LLNode * node){
    if(node==ll->head){
        ll->head = node->next;
        if(ll->head!=NULL)
            ll->head->prev = NULL;
        else ll->tail = NULL;
    }
    else if(node==ll->tail){
        ll->tail = node->prev;
        ll->tail->next = NULL;
    }
    else{
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    ll->count--;
}

/*
    * Function: freeLinkedList
    * -------------------------------
    *  Frees the linked list
    *  list: linked list to be freed
    *  Time complexity: O(n)
    *  n is number of nodes in the linked list
*/
void freeLinkedList(LinkedList * list){
    LLNode * temp = list->head;
    while(temp!=NULL){
        LLNode * temp2 = temp->next;
        free(temp);
        temp = temp2;
    }
    free(list);
}