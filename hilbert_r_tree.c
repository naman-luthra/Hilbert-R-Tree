#include "hilbert_r_tree.h"

int rotate(int n, int *x, int *y, int rx, int ry)
{
    if (ry == 0)
    {
        if (rx == 1)
        {
            *x = n - 1 - *x;
            *y = n - 1 - *y;
        }
        int t = *x;
        *x = *y;
        *y = t;
    }
    return 0;
}

int calculateHilbertValue(rect r){
    int x = (r.minDim[0] + r.maxDim[0])/2;
    int y = (r.minDim[1] + r.maxDim[1])/2;
    int hilbertValue = 0;
    int s = 0;
    for(int s = 1; s < 32; s *= 2){
        int rx = (x & s) > 0;
        int ry = (y & s) > 0;
        hilbertValue += s * s * ((3 * rx) ^ ry);
        rotate(s, &x, &y, rx, ry);
    }
    return hilbertValue;
}

HRTNode * createNewNode(int type)
{
    HRTNode *n = (HRTNode *) malloc(sizeof(HRTNode));
    n->type = type;
    n->count = 0;
    n->parent = NULL;
    n->maxHilbertValue = 0;
    for (int i = 0; i < DIMENSIONS; i++)
    {
        (n->maxBoundingRect).maxDim[i] = INT_MIN;
        (n->maxBoundingRect).minDim[i] = INT_MAX;
    }
    return n;
}

hilbertRTree *createHilbertRTree()
{
    hilbertRTree *hrt = (hilbertRTree *) malloc(sizeof(hilbertRTree));
    hrt->root = createNewNode(LEAFNODE);
    return hrt;
}

void freeNode(HRTNode *n)
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

void searchHRTNode(HRTNode * node, rect queryRect){
    if (!rectangleIntersects(node->maxBoundingRect, queryRect))
        return;

    if (node->type == LEAFNODE)
    {
        for (int i = 0; i < node->count; i++)
        {
            if (rectangleIntersects(node->datapoints[i]->r, queryRect))
            {
                printf("Present in rectangle with minimum DIMENSIONSs (%f, %f) and maximum DIMENSIONSs (%f, %f)\n", 
                    node->datapoints[i]->r.minDim[0], 
                    node->datapoints[i]->r.minDim[1], 
                    node->datapoints[i]->r.maxDim[0], 
                    node->datapoints[i]->r.maxDim[1]
                );
            }
        }
    }
    else
    {
        for (int i = 0; i < node->count; i++)
        {
            if (rectangleIntersects(node->children[i]->maxBoundingRect, queryRect))
            {
                searchHRTNode(node->children[i], queryRect);
            }
        }
    }
}

void searchHRT(hilbertRTree *hrt, rect queryRect)
{
    return searchHRTNode(hrt->root, queryRect);
}

HRTNode *chooseLeaf(hilbertRTree *hrt, int h)
{
    HRTNode * N = hrt->root;
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

void insertToHRTnode(HRTNode* n, void * new){
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
        HRTNode * newNode = new;
        n->children[n->count] = newNode;
        newNode->parent = n;
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

LinkedList * handleOverflow(HRTNode* n, void * new){
    bool allFull = true;
    LinkedList * Nodell = createLinkedList();
    if(n->parent==NULL){
        HRTNode* newNode = createNewNode(NONLEAFNODE);
        insertToHRTnode(newNode,n);
        llInsert(Nodell,n);
    }
    else{
        for(int i = 0; i < n->parent->count; i++){
            llInsert(Nodell, n->parent->children[i]);
            if(n->parent->children[i]->count!=ORDER){
                allFull = false;
                break;
            }
        }
    }
    if(allFull){
        HRTNode* newNode = createNewNode(n->type);
        newNode->count = -1;
        llInsert(Nodell,newNode);
    }

    LinkedList * Childrenll = createLinkedList();
    LLNode * curr = Nodell->head;
    if(n->type==LEAFNODE){
        spatialData * newSD = new;
        bool inserted = false;
        while(curr!=NULL){
            HRTNode * temp = curr->data;
            for(int i = 0; i < temp->count; i++){
                if(!inserted && temp->datapoints[i]->hilbertValue > newSD->hilbertValue){
                    llInsert(Childrenll, newSD);
                    inserted = true;
                }
                llInsert(Childrenll, temp->datapoints[i]);
                temp->datapoints[i] = NULL;
                temp->count--;
            }
            curr = curr->next;
        }
    }
    else{
        HRTNode * newNode = new;
        bool inserted = false;
        while(curr!=NULL){
            HRTNode * temp = curr->data;
            for(int i = 0; i < temp->count; i++){
                if(!inserted && temp->children[i]->maxHilbertValue > newNode->maxHilbertValue){
                    llInsert(Childrenll, newNode);
                    inserted = true;
                }
                llInsert(Childrenll, temp->children[i]);
                temp->children[i] = NULL;
                temp->count--;
            }
            curr = curr->next;
        }
    }

    int childrenPerNode = Childrenll->count/Nodell->count;

    LLNode * currNode = Nodell->head;
    LLNode * currChild = Childrenll->head;

    while(currNode!=NULL){
        HRTNode * temp = currNode->data;
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

    return Nodell;
}

void updateMBRandHV(HRTNode * p){
    for(int i = 0; i < DIMENSIONS; i++){
        p->maxBoundingRect.minDim[i] = INT_MAX;
        p->maxBoundingRect.maxDim[i] = INT_MIN;
    }
    for(int i = 0; i < p->count; i++){
        if(p->type==LEAFNODE){
            spatialData * temp = p->datapoints[i];
            for(int j = 0; j < DIMENSIONS; j++){
                if(temp->r.minDim[j]<p->maxBoundingRect.minDim[j])
                    p->maxBoundingRect.minDim[j] = temp->r.minDim[j];
                if(temp->r.maxDim[j]>p->maxBoundingRect.maxDim[j])
                    p->maxBoundingRect.maxDim[j] = temp->r.maxDim[j];
            }
            if(temp->hilbertValue>p->maxHilbertValue)
                p->maxHilbertValue = temp->hilbertValue;
        }
        else{
            HRTNode * temp = p->children[i];
            for(int j = 0; j < DIMENSIONS; j++){
                if(temp->maxBoundingRect.minDim[j]<p->maxBoundingRect.minDim[j])
                    p->maxBoundingRect.minDim[j] = temp->maxBoundingRect.minDim[j];
                if(temp->maxBoundingRect.maxDim[j]>p->maxBoundingRect.maxDim[j])
                    p->maxBoundingRect.maxDim[j] = temp->maxBoundingRect.maxDim[j];
            }
            if(temp->maxHilbertValue>p->maxHilbertValue)
                p->maxHilbertValue = temp->maxHilbertValue;
        }
    }
}

void adjustTree(LinkedList * affectedNodes)
{
    if(affectedNodes->count==0)
        return;
    HRTNode * firstNode = affectedNodes->head->data;
    HRTNode * lastNode = affectedNodes->tail->data;
    if(lastNode->count==-1){
        lastNode->count=0;
        if(firstNode->parent->count==ORDER)
            adjustTree(handleOverflow(firstNode->parent, lastNode));
        else{
            insertToHRTnode(firstNode->parent, lastNode);
        }
    }
    LinkedList * affectedParents = createLinkedList();
    LLNode * curr = affectedNodes->head;
    while(curr!=NULL){
        HRTNode * temp = curr->data;
        if(temp->parent!=NULL){
            updateMBRandHV(temp->parent);
            llInsert(affectedParents, temp->parent);
        }
        curr = curr->next;
    }
    adjustTree(affectedParents);
}

void insertToHRT(hilbertRTree * hrt, spatialData *sd){
    printf("Inserting (%f, %f) with hilbert value %d\n", sd->r.minDim[0], sd->r.minDim[1], sd->hilbertValue);
    HRTNode * l = chooseLeaf(hrt, sd->hilbertValue);
    LinkedList * affectedNodes;
    if (l->count == ORDER)
        affectedNodes = handleOverflow(l, sd);
    else{
        insertToHRTnode(l, sd);
        affectedNodes = createLinkedList();
        llInsert(affectedNodes, l);
    }
    adjustTree(affectedNodes);
}

void preorderHilbert(HRTNode *root)
{
    if (root->type == NONLEAFNODE)
    {
        printf("NONLEAFNODE: MBR bottom (%f, %f), top (%f, %f)\n", root->maxBoundingRect.minDim[0], root->maxBoundingRect.minDim[1], root->maxBoundingRect.maxDim[0], root->maxBoundingRect.maxDim[1]);
        for(int i = 0; i < root->count; i++){
            preorderHilbert(root->children[i]);
        }
    }
    else
    {
        printf("LEAFNODE: DATAITEMS ");
        for(int i = 0; i < root->count; i++){
            printf("(%f, %f), ", root->datapoints[i]->r.maxDim[0], root->datapoints[i]->r.maxDim[1]);
        }
        printf("\n");
    }
}