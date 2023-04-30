#define ORDER 4
#define DIMENSION 2
#define LEAFNODE 0
#define NONLEAFNODE 1
#define INT_MAX 2147483647

#define max(a, b) ((a>b?a:b))
#define min(a, b) ((a<b?a:b))

typedef struct rectangle{
    double maxDim[DIMENSION];
    double minDim[DIMENSION];
    int hilbertVal;
}rect;

typedef struct node{
    int type;
    int count;
    rect r;
    struct node* parent;
    struct node* left;
    struct node* right;
    int maxHilbertValue;
    union
    {
        rect datapoints[ORDER];
        struct node* children[ORDER];
    };
}node;