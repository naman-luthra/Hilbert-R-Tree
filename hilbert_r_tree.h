#include "hilbert_r_tree_ds.h"

void searchHRT(hilbertRTree * hrt, rect queryRect);

void insertToHRT(hilbertRTree * hrt, spatialData *sd);

hilbertRTree* createHilbertRTree();

void preorderHilbert(HRTNode* root);