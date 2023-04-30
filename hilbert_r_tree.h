#include "hilbert_r_tree_ds.h"

LinkedList * searchHRT(hilbertRTree * hrt, rect queryRect);

void insertToHRT(hilbertRTree * hrt, spatialData *sd);

hilbertRTree* createHilbertRTree();

void preorderHilbert(hilbertRTree* tree);

long long int preorderHRTNode(HRTNode* root);