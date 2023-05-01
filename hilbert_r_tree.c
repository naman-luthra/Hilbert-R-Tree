#include "hilbert_r_tree.h"

/*
    * Function: calculateHilbertValue
    * -------------------------------
    *  Calculates the hilbert value of a rectangle
    *  r: rectangle whose hilbert value is to be calculated
    *  Time complexity: O(log(GRIDSIZE))
*/
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

/*
    * Function: createNewNode
    * -------------------------------
    *  Creates a new node of type LEAFNODE or NONLEAFNODE
    *  type: type of node to be created
    *  Time complexity: O(1)
*/
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

/*
    * Function: createHilbertRTree
    * -------------------------------
    *  Creates a new hilbert r tree
    *  Time complexity: O(1)
*/
hilbertRTree *createHilbertRTree(){
    hilbertRTree *hrt = (hilbertRTree *) malloc(sizeof(hilbertRTree));
    hrt->root = createNewNode(LEAFNODE);
    return hrt;
}

/*
    * Function: rectangleIntersects
    * -------------------------------
    * Checks if two rectangles intersect
    * target: first rectangle
    * r: second rectangle
    * Time complexity: O(1)
*/
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

/*
    * Function: recursiveHRTSearch
    * -------------------------------
    * Recursively searches for all datapoints in a rectangle
    * node: root of HRT which is to be searched
    * queryRect: rectangle in which datapoints are to be searched
    * result: linked list in which results are to be stored
*/
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

/*
    * Function: searchHRT
    * -------------------------------
    * Searches for all datapoints in a rectangle and returns them as a linked list
    * hrt: hilbert r tree which is to be searched
    * queryRect: rectangle in which datapoints are to be searched
*/
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

/*
    * Function: chooseLeaf
    * -------------------------------
    * Chooses a leaf node in which a new datapoint is to be inserted
    * hrt: hilbert r tree in which datapoint is to be inserted
    * h: hilbert value of the datapoint
    * Time complexity: O(height of tree)
*/
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

/*
    * Function: insertToHRTnode
    * -------------------------------
    * Inserts a new datapoint to a node
    * n: node in which datapoint is to be inserted
    * new: datapoint to be inserted
    * Time complexity: O(n)
    * n is number of entries in the node
*/
void insertToHRTnode(HRTNode* n, void * new){
    if(n->type==LEAFNODE){
        spatialData * newSD = new;
        bool inserted = false;
        int ct = n->count;
        for(int i = 0; i < ct; i++){
            if(!inserted && n->datapoints[i]->hilbertValue > newSD->hilbertValue){
                spatialData * temp = n->datapoints[i];
                n->datapoints[i] = newSD;
                newSD = temp;
                inserted = true;
            }
            else if(inserted){
                spatialData * temp = n->datapoints[i];
                n->datapoints[i] = newSD;
                newSD = temp;
            }
        }
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
        newNode->parent = n;
        bool inserted = false;
        int ct = n->count;
        for(int i = 0; i < ct; i++){
            if(!inserted && n->children[i]->maxHilbertValue > newNode->maxHilbertValue){
                HRTNode * temp = n->children[i];
                n->children[i] = newNode;
                newNode = temp;
                inserted = true;
            }
            else if(inserted){
                HRTNode * temp = n->children[i];
                n->children[i] = newNode;
                newNode = temp;
            }
        }
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

/*
    * Function: handleOverflow
    * -------------------------------
    * Handles overflow in a node
    * n: node in which overflow is to be handled
    * new: datapoint to be inserted
    * Time complexity: O(n)
    * n is number of entries in the node and its cooperating siblings
*/
LinkedList * handleOverflow(HRTNode* n, void * new){
    bool allFull = true, split=false;
    LinkedList * Nodell = createLinkedList();
    if(n->parent==NULL){
        HRTNode* newNode = createNewNode(NONLEAFNODE);
        insertToHRTnode(newNode,n);
        llInsert(Nodell,n);
    }
    else{
        int pos;
        for(int i = 0; i < n->parent->count; i++)
            if(n->parent->children[i]==n){
                pos = i;
                break;
            }
        int s = max(0,pos-SPLITTING+1), empty = 0;
        for(int i = s; i < min(s+SPLITTING, n->parent->count); i++)
            empty += ORDER - n->parent->children[i]->count;
        
        int optimalWindow = s, MaxEmpty = empty;
        for(int e = s+SPLITTING; e < n->parent->count; e++){
           empty += ORDER - n->parent->children[e]->count - n->parent->children[e-SPLITTING]->count;
           if(empty > MaxEmpty){
               MaxEmpty = empty;
               optimalWindow = e-SPLITTING+1;
           }
        }
        for(int i = optimalWindow; i < min(optimalWindow+SPLITTING,n->parent->count); i++)
            llInsert(Nodell,n->parent->children[i]);
        if(MaxEmpty > 0) allFull = false;
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

/*
    * Function: updateMBRandHV
    * -------------------------------
    * Recalculates and updates the max bounding rectangle and max hilbert value of a node
    * p: node whose max bounding rectangle and max hilbert value is to be updated
    * Time complexity: O(n)
    * n is number of entries in the node
*/
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

/*
    * Function: adjustTree
    * -------------------------------
    * Adjusts the tree after an insertion or deletion
    * hrt: hilbertRTree to be adjusted
    * affectedNodes: list of nodes affected by the insertion or deletion
    * Time complexity: O(n + h)
    * n is number of nodes affected by the insertion or deletion
    * h is height of the tree
*/
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

/*
    * Function: insertToHRT
    * -------------------------------
    * Inserts a datapoint into the hilbertRTree
    * hrt: hilbertRTree to be inserted into
    * sd: spatial data point to be inserted
    * Time complexity: O(s*M + h)
    * M is maximum number of entries in a node
    * s is number of cooperating siblings allowed
    * h is height of the tree
*/
void insertToHRT(hilbertRTree * hrt, spatialData *sd){
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
}

/*
    * Function: preorderHRTNode
    * -------------------------------
    * Prints the preorder of the hibleRTree rooted at a node
    * root: root of the hilbertRTree
    * Time complexity: O(n*M)
    * n is number of nodes in the tree
    * M is maximum number of entries in a node
*/
long long int totalDataItems = 0, totalLeafNodes = 0;
void preorderHRTNode(HRTNode *root){
    if (root->type == NONLEAFNODE)
    {
        printf("NONLEAFNODE: MBR bottom (%f, %f), top (%f, %f)\n", root->maxBoundingRect.minDim[0], root->maxBoundingRect.minDim[1], root->maxBoundingRect.maxDim[0], root->maxBoundingRect.maxDim[1]);
        for(int i = 0; i < root->count; i++){
            preorderHRTNode(root->children[i]);
        }
    }
    else
    {
        totalLeafNodes++;
        printf("LEAFNODE: DATAITEMS ");
        for(int i = 0; i < root->count; i++){
            printf("(%f, %f), ", root->datapoints[i]->r.maxDim[0], root->datapoints[i]->r.maxDim[1]);
        }
        printf("\n");
        totalDataItems += root->count;
    }
}

/*
    * Function: preorderHilbert
    * -------------------------------
    * Prints the preorder of the hibleRTree
    * tree: hilbertRTree to be printed
    * Time complexity: O(n*M)
    * n is number of nodes in the tree
    * M is maximum number of entries in a node
*/
void preorderHilbert(hilbertRTree * tree)
{
    totalDataItems = 0;
    totalLeafNodes = 0;
    preorderHRTNode(tree->root);
    printf("\n\nTotal datapoints triversed %lld\n", totalDataItems);
    printf("Total leaf nodes triversed %lld\n", totalLeafNodes);
    printf("Utilization %f%%\n", ((float)totalDataItems*100)/(totalLeafNodes*ORDER));
}