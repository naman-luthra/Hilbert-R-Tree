#ifndef HILBERT_R_TREE_H
#define HILBERT_R_TREE_H

#include "hilbert_r_tree_ds.h"

/*
    * Function: searchHRT
    * -------------------------------
    * Searches for all datapoints in a rectangle
    * hrt: hilbert r tree which is to be searched
    * queryRect: rectangle in which datapoints are to be searched
*/
LinkedList * searchHRT(hilbertRTree * hrt, rect queryRect);

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
void insertToHRT(hilbertRTree * hrt, spatialData *sd);

/*
    * Function: createHilbertRTree
    * -------------------------------
    *  Creates a new hilbert r tree
    *  Time complexity: O(1)
*/
hilbertRTree* createHilbertRTree();

/*
    * Function: preorderHilbert
    * -------------------------------
    * Prints the preorder of the hibleRTree
    * tree: hilbertRTree to be printed
    * Time complexity: O(n*M)
    * n is number of nodes in the tree
    * M is maximum number of entries in a node
*/
void preorderHilbert(hilbertRTree* tree);

#endif