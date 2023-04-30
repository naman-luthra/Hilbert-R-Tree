#define ORDER 4
#define DIMENSIONS 2
#define LEAFNODE 0
#define NONLEAFNODE 1
#define INT_MAX 2147483647
#define BUFFERSIZE 1024

#define max(a, b) ((a>b?a:b))
#define min(a, b) ((a<b?a:b))

typedef struct rectangle{
    double maxDim[DIMENSIONS];
    double minDim[DIMENSIONS];
} rect;

typedef struct spatialData{
    void * data;
    rect r;
    int hilbertValue;
} spatialData;

typedef struct node{
    int type;
    int count;
    rect maxBoundingRect;
    struct node* parent;
    struct node* left;
    struct node* right;
    int maxHilbertValue;
    union
    {
        spatialData * datapoints[ORDER];
        struct node * children[ORDER];
    };
} node;

typedef struct hilbertRTree{
    node* root;
} hilbertRTree;