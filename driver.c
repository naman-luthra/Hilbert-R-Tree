#include <stdio.h>
#include <string.h>
#include "hilbert_r_tree.c"

int main(){
    FILE* fp = fopen("test.txt", "r");
    hilbertRTree* hrt = createHilbertRTree();
    char buffer[1024];
    while(fgets(buffer, BUFFERSIZE, fp)!=NULL){
        char* symRead = strtok(buffer, "\n");
        while(symRead!=NULL){
            spatialData* ds = (spatialData*)malloc(sizeof(spatialData));
            int x = 0, y = 0;
            int i = 0;
            while(symRead[i]>='0'&&symRead[i]<='9'){
                x = x*10 + symRead[i] - '0';
                i++;
            }
            i++;
            while(symRead[i]>='0'&&symRead[i]<='9'){
                y = y*10 + symRead[i]-'0';
            }
            ds->r.maxDim[0] = x;
            ds->r.maxDim[1] = y;
            ds->r.minDim[0] = x;
            ds->r.minDim[1] = y;
            ds->hilbertValue = calculateHilbertValue(x, y);
            insert(hrt->root, ds);
            symRead = strtok(NULL, "\n");
        }
    }
    preorderHilbert(hrt->root);
}