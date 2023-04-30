#include <stdlib.h>
#include "linkedlist.h"
#include "hilbert_r_tree.h"

int calculateHilbertValue(int x, int y)
{
    return INT_MAX;
}

node *createNewNode()
{
    node *n = (node *)malloc(sizeof(node));
    n->type = LEAFNODE;
    n->count = 0;
    n->left = NULL;
    n->right = NULL;
    n->parent = NULL;
    n->maxHilbertValue = 0;
    for (int i = 0; i < DIMENSIONS; i++)
    {
        (n->maxBoundingRect).maxDim[i] = 0;
        (n->maxBoundingRect).minDim[i] = 0;
    }
    return n;
}

hilbertRTree *createHilbertRTree()
{
    hilbertRTree *hrt = (hilbertRTree *)malloc(sizeof(hilbertRTree));
    hrt->root = createNewNode();
    return hrt;
}

void freeNode(node *n)
{
    if (n->type == LEAFNODE)
    {
        free(n);
    }
    else
    {
        for (int i = 0; i < n->count; i++)
        {
            free(n->children[i]);
        }
        free(n);
    }
}

bool rectangleIntersects(rect target, rect r)
{
    for (int i = 0; i < DIMENSIONS; i++)
    {
        if (target.minDim[i] > r.maxDim[i] || target.maxDim[i] < r.minDim[i])
        {
            return false;
        }
    }
    return true;
}

void search(hilbertRTree *hrt, rect queryRect)
{
    if (!rectangleIntersects(hrt->root->maxBoundingRect, queryRect))
        return;

    if (hrt->root->type == LEAFNODE)
    {
        for (int i = 0; i < hrt->root->count; i++)
        {
            if (rectangleIntersects(hrt->root->datapoints[i]->r, queryRect))
            {
                printf("Present in rectangle with minimum DIMENSIONSs (%d, %d) and maximum DIMENSIONSs (%d, %d)\n", hrt->root->datapoints[i]->r.minDim[0], hrt->root->datapoints[i]->r.minDim[1], hrt->root->datapoints[i]->r.maxDim[0], hrt->root->datapoints[i]->r.maxDim[1]);
            }
        }
    }
    else
    {
        for (int i = 0; i < hrt->root->count; i++)
        {
            if (rectangleIntersects(hrt->root->children[i]->maxBoundingRect, queryRect))
            {
                search(hrt->root->children[i], queryRect);
            }
        }
    }
}

node *chooseLeaf(hilbertRTree *hrt, int h)
{
    node *N = hrt->root;

    while (N->type != LEAFNODE)
    {
        for (int i = 0; i < N->count; i++)
        {
            if (N->children[i]->maxHilbertValue > h || i == N->count - 1)
            {
                N = N->children[i];
                break;
            }
        }
    }

    return N;
}

void insertToHRTnode(node* n, void * new){
    if(n->type==LEAFNODE){
        spatialData * newSD = new;
        n->datapoints[n->count] = newSD;
        n->count++;
        for(int i = 0; i < DIMENSIONS; i++){
            if(newSD->r.minDim[i]<n->maxBoundingRect.minDim[i])
                n->maxBoundingRect.minDim[i] = newSD->r.minDim[i];
            if(newSD->r.maxDim[i]>n->maxBoundingRect.maxDim[i])
                n->maxBoundingRect.maxDim[i] = newSD->r.maxDim[i];
        }
        if(newSD->hilbertValue>n->maxHilbertValue)
            n->maxHilbertValue = newSD->hilbertValue;
    }
    else{
        node * newNode = new;
        n->children[n->count] = newNode;
        n->count++;
        for(int i = 0; i < DIMENSIONS; i++){
            if(newNode->maxBoundingRect.minDim[i]<n->maxBoundingRect.minDim[i])
                n->maxBoundingRect.minDim[i] = newNode->maxBoundingRect.minDim[i];
            if(newNode->maxBoundingRect.maxDim[i]>n->maxBoundingRect.maxDim[i])
                n->maxBoundingRect.maxDim[i] = newNode->maxBoundingRect.maxDim[i];
        }
        if(newNode->maxHilbertValue>n->maxHilbertValue)
            n->maxHilbertValue = newNode->maxHilbertValue;
    }
}

node * handleOverflow(node* n, void * new){
    bool allFull = true;
    node* createdNode = NULL;
    LinkedList * Nodell = createLinkedList();
    for(int i = 0; i < n->parent->count; i++){
        llInsert(Nodell, n->parent->children[i]);
        if(n->parent->children[i]->count!=ORDER){
            allFull = false;
            break;
        }
    }
    if(allFull){
        node* newNode = createNewNode();
        newNode->parent = n->parent;
        newNode->type = n->type;
        llInsert(Nodell,newNode);
        createdNode = newNode;
    }

    LinkedList * Childrenll = createLinkedList();
    LLNode * curr = Nodell->head;
    if(n->type==LEAFNODE){
        spatialData * newSD = new;
        bool inserted = false;
        while(curr!=NULL){
            node * temp = curr->data;
            for(int i = 0; i < temp->count; i++){
                if(!inserted && temp->datapoints[i]->hilbertValue > newSD->hilbertValue){
                    llInsert(Childrenll, newSD);
                    inserted = true;
                }
                llInsert(Childrenll, temp->datapoints[i]);
                temp->datapoints[i] = NULL;
            }
            curr = curr->next;
        }
    }
    else{
        node * newNode = new;
        bool inserted = false;
        while(curr!=NULL){
            node * temp = curr->data;
            for(int i = 0; i < temp->count; i++){
                if(!inserted && temp->children[i]->maxHilbertValue > newNode->maxHilbertValue){
                    llInsert(Childrenll, newNode);
                    inserted = true;
                }
                llInsert(Childrenll, temp->children[i]);
                temp->children[i] = NULL;
            }
            curr = curr->next;
        }
    }

    int childrenPerNode = Childrenll->count/Nodell->count;

    LLNode * currNode = Nodell->head;
    LLNode * currChild = Childrenll->head;

    while(currNode!=NULL){
        node * temp = currNode->data;
        for(int i = 0; i < childrenPerNode; i++){
            insertToHRTnode(temp, currChild->data);
            currChild = currChild->next;
        }
        if(currNode->next==NULL){
            while(currChild!=NULL){
                insertToHRTnode(temp, currChild->data);
                currChild = currChild->next;
            }
        }
        currNode = currNode->next;
    }

    return createdNode;
}

void adjustTree(node *n)
{
    if (n->parent == NULL)
    {
        return;
    }
    node *par = n->parent;
}

void insert(node *root, spatialData *sd)
{
    node *l = chooseLeaf(root, sd->hilbertValue);
    if (l->count == ORDER)
        handleOverflow(l, sd);
    else
    {
        int index = 0;
        for (; index < root->count; index++)
        {
            if (root->maxHilbertValue > sd->hilbertValue)
            {
                break;
            }
        }
        for (int i = root->count - 1; i >= index; i++)
        {
            root->datapoints[i + 1] = root->datapoints[i];
        }
        root->datapoints[index] = sd;
    }
}

void preorderHilbert(node *root)
{
    if (root->type == NONLEAFNODE)
    {
        printf("NONLEAFNODE: MBR bottom (%d, %d), top (%d, %d)\n", root->maxBoundingRect.minDim[0], root->maxBoundingRect.minDim[1], root->maxBoundingRect.maxDim[0], root->maxBoundingRect.maxDim[1]);
        for(int i = 0; i < root->count; i++){
            preorderHilbert(root->children[i]);
        }
    }
    else
    {
        printf("LEAFNODE: DATAITEMS ");
        for(int i = 0; i < root->count; i++){
            printf("(%d, %d), ", root->datapoints[i]->r.maxDim[0], root->datapoints[i]->r.maxDim[1]);
        }
        printf("\n");
    }
}