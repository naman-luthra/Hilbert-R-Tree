#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linkedlist.c"
#include "hilbert_r_tree.c"

int main(){
    FILE* fp = fopen("test.txt", "r");
    hilbertRTree* hrt = createHilbertRTree();
    char buffer[1024];
    while(fgets(buffer, BUFFERSIZE, fp)!=NULL){
        char* symRead = strtok(buffer, "\n");
        while(symRead!=NULL){
            spatialData * sd = (spatialData *) malloc(sizeof(spatialData));
            double x = 0, y = 0;
            int i = 0;
            while(symRead[i]>='0'&&symRead[i]<='9'){
                int temp = symRead[i] - '0';
                x = x*10 + temp;
                i++;
            }
            i++;
            while(i!=strlen(symRead)&&symRead[i]>='0'&&symRead[i]<='9'){
                int temp = symRead[i] - '0';
                y = y*10 + temp;
                i++;
            }
            sd->r.maxDim[0] = x;
            sd->r.maxDim[1] = y;
            sd->r.minDim[0] = x;
            sd->r.minDim[1] = y;
            sd->hilbertValue = calculateHilbertValue(sd->r);

            insertToHRT(hrt, sd);
            symRead = strtok(NULL, "\n");
        }
    }
    printf("Preorder traversal of the tree:\n");
    preorderHilbert(hrt->root);
}