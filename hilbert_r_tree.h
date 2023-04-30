#include "hilbert_r_tree_ds.h"
#include <stdbool.h>

int calcHilbertValue(int x, int y);

void search(node* root, rect r);

void insert(node* root, rect r);

node* createNewNode();

node* initTree();

void freeNode(node* n);