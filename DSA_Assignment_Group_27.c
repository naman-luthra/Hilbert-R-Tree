#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define ORDER 4
#define DIMENSIONS 2
#define LEAFNODE 0
#define NONLEAFNODE 1
#define INT_MAX 2147483647
#define INT_MIN -2147483648
#define BUFFERSIZE 1024
#define GRIDSIZE 1048576
#define SPLITTING 4

#define max(a, b) ((a>b?a:b))
#define min(a, b) ((a<b?a:b))

/*
    * Struct: LLNode
    * ------------------
    * data: pointer to the data
    * next: pointer to the next node
    * prev: pointer to the previous node
*/
typedef struct LLNode {
    void * data;
    struct LLNode * next;
    struct LLNode * prev;
} LLNode;

/*
    * Struct: LinkedList
    * ------------------
    * head: pointer to the head of the linked list
    * tail: pointer to the tail of the linked list
    * count: number of nodes in the linked list
*/
typedef struct LinkedList {
    LLNode *head;
    LLNode *tail;
    int count;
} LinkedList;

/*
    * Struct: rectangle
    * ------------------
    * maxDim: array of maximum coordinates of the rectangle
    * minDim: array of minimum coordinates of the rectangle
*/
typedef struct rectangle{
    double maxDim[DIMENSIONS];
    double minDim[DIMENSIONS];
} rect;

/*
    * Struct: spatialData
    * ------------------
    * data: pointer to the data
    * r: rectangle in which the data is present
    * hilbertValue: hilbert value of the rectangle
*/
typedef struct spatialData{
    void * data;
    rect r;
    long long int hilbertValue;
} spatialData;

/*
    * Struct: HRTNode
    * ------------------
    * type: type of the node (leaf or non-leaf)
    * count: number of entries in the node
    * minBoundingRect: bounding rectangle of the node
    * parent: pointer to the parent of the node
    * maxHilbertValue: maximum hilbert value of the entries or children of the node
    * datapoints: array of pointers to the spatial data points in case of leaf node
    * children: array of pointers to the children of the node in case of non-leaf node
*/
typedef struct HRTNode{
    int type;
    int count;
    rect minBoundingRect;
    struct HRTNode* parent;
    long long int maxHilbertValue;
    union
    {
        spatialData * datapoints[ORDER];
        struct HRTNode * children[ORDER];
    };
} HRTNode;

/*
    * Struct: hilbertRTree
    * ------------------
    * root: pointer to the root of the tree
*/
typedef struct hilbertRTree{
    HRTNode * root;
} hilbertRTree;


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
        (n->minBoundingRect).maxDim[i] = INT_MIN;
        (n->minBoundingRect).minDim[i] = INT_MAX;
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
    if(!rectangleIntersects(node->minBoundingRect, queryRect))
        return;

    if(node->type == LEAFNODE){
        for(int i = 0; i < node->count; i++)
            if(rectangleIntersects(node->datapoints[i]->r, queryRect))
                llInsert(result, node->datapoints[i]);
    }
    else{
        for(int i = 0; i < node->count; i++)
            if(rectangleIntersects(node->children[i]->minBoundingRect, queryRect))
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
    * Inserts a new datapoint to a a leaf node or a node entry in a non leaf node
    * n: node in which insertion is to be done
    * new: datapoint or node to be inserted
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
            if(newSD->r.minDim[i]<n->minBoundingRect.minDim[i])
                n->minBoundingRect.minDim[i] = newSD->r.minDim[i];
            if(newSD->r.maxDim[i]>n->minBoundingRect.maxDim[i])
                n->minBoundingRect.maxDim[i] = newSD->r.maxDim[i];
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
            if(newNode->minBoundingRect.minDim[i]<n->minBoundingRect.minDim[i])
                n->minBoundingRect.minDim[i] = newNode->minBoundingRect.minDim[i];
            if(newNode->minBoundingRect.maxDim[i]>n->minBoundingRect.maxDim[i])
                n->minBoundingRect.maxDim[i] = newNode->minBoundingRect.maxDim[i];
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
    * new: datapoint or node that caused the overflow
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
        p->minBoundingRect.minDim[i] = INT_MAX;
        p->minBoundingRect.maxDim[i] = INT_MIN;
    }
    for(int i = 0; i < p->count; i++){
        if(p->type==LEAFNODE){
            spatialData * temp = p->datapoints[i];
            for(int j = 0; j < DIMENSIONS; j++){
                if(temp->r.minDim[j]<p->minBoundingRect.minDim[j])
                    p->minBoundingRect.minDim[j] = temp->r.minDim[j];
                if(temp->r.maxDim[j]>p->minBoundingRect.maxDim[j])
                    p->minBoundingRect.maxDim[j] = temp->r.maxDim[j];
            }
            if(temp->hilbertValue>p->maxHilbertValue)
                p->maxHilbertValue = temp->hilbertValue;
        }
        else{
            HRTNode * temp = p->children[i];
            for(int j = 0; j < DIMENSIONS; j++){
                if(temp->minBoundingRect.minDim[j]<p->minBoundingRect.minDim[j])
                    p->minBoundingRect.minDim[j] = temp->minBoundingRect.minDim[j];
                if(temp->minBoundingRect.maxDim[j]>p->minBoundingRect.maxDim[j])
                    p->minBoundingRect.maxDim[j] = temp->minBoundingRect.maxDim[j];
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
        printf("NONLEAFNODE: MBR bottom (%f, %f), top (%f, %f)\n", root->minBoundingRect.minDim[0], root->minBoundingRect.minDim[1], root->minBoundingRect.maxDim[0], root->minBoundingRect.maxDim[1]);
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

int main(int argc, char *argv[]){
    if(argc!=2){
        printf("Usage: ./a.out <input file>\n");
        return 0;
    }
    FILE* fp = fopen(argv[1], "r");
    hilbertRTree* hrt = createHilbertRTree();
    char buffer[1024];
    while(fgets(buffer, BUFFERSIZE, fp)!=NULL){
        char* symRead = strtok(buffer, "\n");
        while(symRead!=NULL){
            spatialData * sd = (spatialData *) malloc(sizeof(spatialData));
            double x = 0, y = 0;
            int i = 0;
            while(symRead[i]>='0'&&symRead[i]<='9'){
                int temp = symRead[i] - '0';
                x = x*10 + temp;
                i++;
            }
            i++;
            while(i!=strlen(symRead)&&symRead[i]>='0'&&symRead[i]<='9'){
                int temp = symRead[i] - '0';
                y = y*10 + temp;
                i++;
            }
            sd->r.maxDim[0] = x;
            sd->r.maxDim[1] = y;
            sd->r.minDim[0] = x;
            sd->r.minDim[1] = y;
            sd->hilbertValue = calculateHilbertValue(sd->r);

            insertToHRT(hrt, sd);
            symRead = strtok(NULL, "\n");
        }
    }
    printf("Preorder traversal of the tree:\n");
    preorderHilbert(hrt);

    int choice = 1;
    while(choice!=0){
        printf("Choose from given options\n\n");
        printf("- To make a query, enter 1\n");
        printf("- To exit, enter 0\n\n");
        scanf("%d", &choice);
        printf("\n");
        switch (choice){
            case 1:
                printf("Enter the query rectangle in the format: x1 y1 x2 y2: ");
                double x1, y1, x2, y2;
                scanf("%lf %lf %lf %lf", &x1, &y1, &x2, &y2);
                rect queryRect;
                queryRect.maxDim[0] = max(x1, x2);
                queryRect.maxDim[1] = max(y1, y2);
                queryRect.minDim[0] = min(x1, x2);
                queryRect.minDim[1] = min(y1, y2);
                printf("\n");
                searchHRT(hrt, queryRect);
                break;
            case 0:
                break;
        }
    }
    return 0;
}