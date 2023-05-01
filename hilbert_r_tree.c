#include "hilbert_r_tree.h"

long long int calculateHilbertValue(rect r){
    long long int 
        x = (r.minDim[0] + r.maxDim[0])/2,
        y = (r.minDim[1] + r.maxDim[1])/2,
        rx, 
        ry, 
        s=GRIDSIZE/2, 
        hilbertValue = 0;
    while(s>0){
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        hilbertValue += s*s*((3*rx)^ry);
        if(ry==0){
            if (rx == 1) {
                x = GRIDSIZE-1-x;
                y = GRIDSIZE-1-y;
            }
            long long int t  = x;
            x = y;
            y = t;
        }
        s /= 2;
    }
    return hilbertValue;
}

HRTNode * createNewNode(int type){
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

hilbertRTree *createHilbertRTree(){
    hilbertRTree *hrt = (hilbertRTree *) malloc(sizeof(hilbertRTree));
    hrt->root = createNewNode(LEAFNODE);
    return hrt;
}

void freeNode(HRTNode *n){
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

bool rectangleIntersects(rect target, rect r){
    for (int i = 0; i < DIMENSIONS; i++)
    {
        if (target.minDim[i] > r.maxDim[i] || target.maxDim[i] < r.minDim[i])
        {
            return false;
        }
    }
    return true;
}

void recursiveHRTSearch(HRTNode * node, rect queryRect, LinkedList * result){
    if(!rectangleIntersects(node->maxBoundingRect, queryRect))
        return;

    if(node->type == LEAFNODE){
        for(int i = 0; i < node->count; i++)
            if(rectangleIntersects(node->datapoints[i]->r, queryRect))
                llInsert(result, node->datapoints[i]);
    }
    else{
        for(int i = 0; i < node->count; i++)
            if(rectangleIntersects(node->children[i]->maxBoundingRect, queryRect))
                recursiveHRTSearch(node->children[i], queryRect, result);
    }
}


LinkedList * searchHRT(hilbertRTree *hrt, rect queryRect){
    LinkedList * result = createLinkedList();
    recursiveHRTSearch(hrt->root, queryRect, result);
    printf("Found %d results\n\n", result->count);

    LLNode * current = result->head;
    while(current != NULL){
        spatialData * sd = current->data;
        printf("Found [(%f,%f),(%f,%f)]\n", sd->r.minDim[0], sd->r.minDim[1], sd->r.maxDim[0], sd->r.maxDim[1]);
        current = current->next;
    }

    return result;
}

HRTNode *chooseLeaf(hilbertRTree *hrt, int h){
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
    // printf("Overflow %d children of type %d \n", n->count, n->type);
    bool allFull = true, split=false;
    LinkedList * Nodell = createLinkedList();
    if(n->parent==NULL){
        // printf("Creating new root\n");
        HRTNode* newNode = createNewNode(NONLEAFNODE);
        insertToHRTnode(newNode,n);
        llInsert(Nodell,n);
    }
    else{
        for(int i = 0; i < n->parent->count; i++){
            llInsert(Nodell, n->parent->children[i]);
            if(n->parent->children[i]->count!=ORDER){
                allFull = false;
            }
        }
    }
    if(allFull){
        HRTNode* newNode = createNewNode(n->type);
        llInsert(Nodell,newNode);
        split = true;
    }

    LinkedList * Childrenll = createLinkedList();
    LLNode * curr = Nodell->head;
    if(n->type==LEAFNODE){
        spatialData * newSD = new;
        bool inserted = false;
        while(curr!=NULL){
            HRTNode * temp = curr->data;
            int ct = temp->count;
            for(int i = 0; i < ct; i++){
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
        if(!inserted)
            llInsert(Childrenll, newSD);
    }
    else{
        HRTNode * newNode = new;
        bool inserted = false;
        while(curr!=NULL){
            HRTNode * temp = curr->data;
            int ct = temp->count;
            for(int i = 0; i < ct; i++){
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
        if(!inserted)
            llInsert(Childrenll, newNode);
    }

    // printf("Reordering %d children into %d nodes\n", Childrenll->count, Nodell->count);
    int childrenPerNode = Childrenll->count/Nodell->count;
    int extraChildren = Childrenll->count%Nodell->count;

    LLNode * currNode = Nodell->head;
    LLNode * currChild = Childrenll->head;

    while(currNode!=NULL){
        HRTNode * temp = currNode->data;
        for(int i = 0; i < childrenPerNode; i++){
            insertToHRTnode(temp, currChild->data);
            currChild = currChild->next;
        }
        if(extraChildren){
            insertToHRTnode(temp, currChild->data);
            currChild = currChild->next;
            extraChildren--;
        }
        currNode = currNode->next;
    }
    if(split)
        ((HRTNode *) Nodell->tail->data)->type = 10 + ((HRTNode *) Nodell->tail->data)->type;
    freeLinkedList(Childrenll);
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

void adjustTree(hilbertRTree * hrt, LinkedList * affectedNodes)
{
    if(affectedNodes->count==0)
        return;
    HRTNode * parentLastNode = NULL;
    HRTNode * firstNode = affectedNodes->head->data;
    HRTNode * lastNode = affectedNodes->tail->data;
    if(lastNode->type>=10){
        lastNode->type = lastNode->type - 10;
        if(firstNode->parent->count==ORDER){
            LinkedList * temp = handleOverflow(firstNode->parent, lastNode);
            parentLastNode = temp->tail->data;
            freeLinkedList(temp);
        }
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
        else{
            hrt->root = temp;
        }
        curr = curr->next;
    }
    if(parentLastNode && parentLastNode->type>=10)
        llInsert(affectedParents, parentLastNode);
    adjustTree(hrt, affectedParents);
    freeLinkedList(affectedParents);
}

void insertToHRT(hilbertRTree * hrt, spatialData *sd){
    // printf("Inserting (%f, %f) with hilbert value %lld\n", sd->r.minDim[0], sd->r.minDim[1], sd->hilbertValue);
    HRTNode * l = chooseLeaf(hrt, sd->hilbertValue);
    LinkedList * affectedNodes;
    if (l->count == ORDER){
        affectedNodes = handleOverflow(l, sd);
    }
    else{
        insertToHRTnode(l, sd);
        affectedNodes = createLinkedList();
        llInsert(affectedNodes, l);
    }
    adjustTree(hrt, affectedNodes);
    freeLinkedList(affectedNodes);
    // printf("----------------------\n");
    // printf("%d\n", preorderHRTNode(hrt->root));
    // printf("----------------------\n");
}

long long int preorderHRTNode(HRTNode *root){
    long long int out = 0;
    if (root->type == NONLEAFNODE)
    {
        printf("NONLEAFNODE: MBR bottom (%f, %f), top (%f, %f)\n", root->maxBoundingRect.minDim[0], root->maxBoundingRect.minDim[1], root->maxBoundingRect.maxDim[0], root->maxBoundingRect.maxDim[1]);
        for(int i = 0; i < root->count; i++){
            out += preorderHRTNode(root->children[i]);
        }
    }
    else
    {
        printf("LEAFNODE: DATAITEMS ");
        for(int i = 0; i < root->count; i++){
            printf("(%f, %f), ", root->datapoints[i]->r.maxDim[0], root->datapoints[i]->r.maxDim[1]);
        }
        printf("\n");
        out += root->count;
    }
    return out;
}

void preorderHilbert(hilbertRTree * tree)
{
    printf("\n\nTotal datapoints triversed %lld\n", preorderHRTNode(tree->root));
}