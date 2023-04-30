#include "hilbert_r_tree_ds.h"
#include <stdbool.h>

void search(hilbertRTree* hrt, rect queryRect);

void insert(hilbertRTree * hrt, spatialData *sd);

hilbertRTree* createHilbertRTree();

void preorderHilbert(node* root);