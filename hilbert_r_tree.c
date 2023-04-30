#include <stdlib.h>
#include "hilbert_r_tree.h"

node* createNewNode(){
    node* n = (node*)malloc(sizeof(node));
    n->type = LEAFNODE;
    n->count = 0;
    n->left = NULL;
    n->right = NULL;
    n->parent = NULL;
    n->maxHilbertValue = 0;
    for(int i = 0; i < DIMENSIONS; i++){
        (n->maxBoundingRect).maxDim[i] = 0;
        (n->maxBoundingRect).minDim[i] = 0;
    }
    return n;
}

hilbertRTree* createHilbertRTree(){
    hilbertRTree * hrt = (hilbertRTree*) malloc(sizeof(hilbertRTree));
    hrt->root = createNewNode();
    return hrt;
}

void freeNode(node* n){
    if(n->type==LEAFNODE){
        free(n);
    }
    else{
        for(int i = 0; i < n->count; i++){
            free(n->children[i]);
        }
        free(n);
    }
}

bool rectangleIntersects(rect target, rect r){
    for(int i = 0; i < DIMENSIONS; i++){
        if(target.minDim[i]>r.maxDim[i] || target.maxDim[i]<r.minDim[i]){
            return false;
        }
    }
    return true;
}

void search(hilbertRTree* hrt, rect queryRect){
    if(!rectangleIntersects(hrt->root->maxBoundingRect, queryRect))
        return;

    if(hrt->root->type==LEAFNODE){
        for(int i = 0; i < hrt->root->count; i++){
            if(rectangleIntersects(hrt->root->datapoints[i]->r, queryRect)){
                printf("Present in rectangle with minimum DIMENSIONSs (%d, %d) and maximum DIMENSIONSs (%d, %d)\n"
                ,hrt->root->datapoints[i]->r.minDim[0]
                ,hrt->root->datapoints[i]->r.minDim[1]
                ,hrt->root->datapoints[i]->r.maxDim[0]
                ,hrt->root->datapoints[i]->r.maxDim[1]);
            }
        }
    }
    else{
        for(int i = 0; i < hrt->root->count; i++){
            if(rectangleIntersects(hrt->root->children[i]->maxBoundingRect, queryRect)){
                search(hrt->root->children[i], queryRect);
            }
        }
    }
}

node* chooseLeaf(hilbertRTree* hrt, int h){
    node * N = hrt->root;

    while(N->type!=LEAFNODE){
        for(int i = 0; i < N->count; i++){
            if(N->children[i]->maxHilbertValue>h || i==N->count-1){
                N = N->children[i];
                break;
            }
        }
    }
        
    return N;
}

void handleOverflow(node* leaf, spatialData * sd){

}

void adjustTree(node* n){
    if(n->parent==NULL){
        return;
    }
    node* par = n->parent;
    
}

void insert(node* root, spatialData * sd){
    node* l = chooseLeaf(root, sd->hilbertValue);
    if(l->count==ORDER)
        handleOverflow(l, sd);
    else{
        int index = 0;
        for(; index < root->count; index++){
            if(root->maxHilbertValue>sd->hilbertValue){
                break;
            }
        }
        for(int i = root->count-1; i >= index; i++){
            root->datapoints[i+1] = root->datapoints[i];
        }
        root->datapoints[index] = sd;
    }
}