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
    for(int i = 0; i < DIMENSION; i++){
        (n->r).maxDim[i] = 0;
        (n->r).minDim[i] = 0;
    }
    return n;
}

node* initTree(){
    return createNewNode();
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

bool rectangleIntersects(rect target, rect other){
    for(int i = 0; i < DIMENSION; i++){
        if(target.minDim[i]>other.maxDim[i] || target.maxDim[i]<other.minDim[i]){
            return false;
        }
    }
    return true;
}

bool rectangleIntersectsHilbert(rect target, rect other){
    return target.hilbertVal>=other.hilbertVal;
}

void search(node* root, rect other){
    if(!rectangleIntersects(root->r, other)){
        return false;
    }
    if(root->type==LEAFNODE){
        for(int i = 0; i < root->count; i++){
            if(rectangleIntersects(root->datapoints[i], other)){
                printf("Present in rectangle with minimum dimensions (%d, %d) and maximum dimensions (%d, %d)\n", root->datapoints[i].minDim[0], root->datapoints[i].minDim[1], root->datapoints[i].maxDim[0], root->datapoints[i].maxDim[1]);
            }
        }
    }
    else{
        for(int i = 0; i < root->count; i++){
            if(rectangleIntersects(root->children[i]->r, other)){
                search(root->children[i], other);
            }
        }
    }
}

node* chooseLeaf(node* root, int h){
    if(root->type==LEAFNODE){
        root->r.hilbertVal = max(root->r.hilbertVal, h);
        return root;
    }
    int minHilbertVal = INT_MAX;
    node* selChild = NULL;
    for(int i = 0; i < root->count; i++){
        if(root->children[i]->r.hilbertVal>h){
            if(minHilbertVal==INT_MAX){
                selChild = root->children[i];
            }
            else{
                if(minHilbertVal>root->children[i]->r.hilbertVal){
                    selChild = root->children[i];
                    minHilbertVal = root->children[i]->r.hilbertVal;
                }
            }
        }
    }
    if(selChild==NULL){
        chooseLeaf(root->children[(root->count)-1], h);
    }
    else{
        chooseLeaf(selChild, h);
    }
}

// void manageHilbert(node* n){
//     if(n==NULL){
//         return;
//     }
//     int hilVal = n->r.hilbertVal;
//     for(int i = 0; i < n->count; i++){
//         hilVal = min(hilVal, n->children[i]->r.hilbertVal);
//     }
//     if(n->r.hilbertVal!=hilVal){
//         n->r.hilbertVal = hilVal;
//         manageHilbert(n->parent);
//     }
// }

void handleOverflow(node* leaf, rect other){
    node* leftSibling = leaf->left;
    node* rightSibling = leaf->right;
    bool leftEmpty = false, rightEmpty = false;
    rect minRect, maxRect;
    minRect = (leaf->datapoints)[0];
    maxRect = (leaf->datapoints)[0];
    for(int i = 0; i < leaf->count; i++){
        if(minRect.hilbertVal>leaf->children[i]){
            minRect = leaf->children[i]->r;
        }
        if(maxRect.hilbertVal < leaf->children[i]->r.hilbertVal){
            maxRect = leaf->children[i]->r;
        }
    }
    while(leftSibling!=NULL){
        if(leftSibling->count!=ORDER){
            leftEmpty = true;
            break;
        }
        leftSibling = leftSibling->left;
    }
    while(rightSibling!=NULL){
        if(rightSibling!=NULL){
            rightEmpty = true;
            break;
        }
        rightSibling = rightSibling->right;
    }
    if(leftEmpty){

    }
    if(rightEmpty){

    }
}

void adjustTree(node* n){
    if(n->parent==NULL){
        return;
    }
    node* par = n->parent;
    
}

void insert(node* root, rect other){
    node* l = chooseLeaf(root, other.hilbertVal);
    if(l->count==ORDER){
        handleOverflow(l, other);
    }
    else{
        if(l->count==0){
            (l->datapoints)[l->count++] = other;
            l->r.hilbertVal = max(l->r.hilbertVal, other.hilbertVal);
            adjustTree(l);
        }
        else{
            int st = 0;
            int en = l->count-1;
            while(st < en){
                int mid = st + (en - st)/2;
                if((l->datapoints)[mid].hilbertVal<other.hilbertVal){
                    st = mid+1;
                }
                else{
                    en = mid-1;
                }
            }
        }
    }

}