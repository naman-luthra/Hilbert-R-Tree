#ifndef HILBERT_R_TREE_DS_H
#define HILBERT_R_TREE_DS_H

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

typedef struct rectangle{
    double maxDim[DIMENSIONS];
    double minDim[DIMENSIONS];
} rect;

typedef struct spatialData{
    void * data;
    rect r;
    long long int hilbertValue;
} spatialData;

typedef struct HRTNode{
    int type;
    int count;
    rect maxBoundingRect;
    struct HRTNode* parent;
    long long int maxHilbertValue;
    union
    {
        spatialData * datapoints[ORDER];
        struct HRTNode * children[ORDER];
    };
} HRTNode;

typedef struct hilbertRTree{
    HRTNode * root;
} hilbertRTree;

#endif