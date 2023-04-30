#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linkedlist.c"
#include "hilbert_r_tree.c"

int main(){
    FILE* fp = fopen("bigtest.txt", "r");
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
    preorderHilbert(hrt);

    int choice = 1;
    while(choice!=0){
        printf("Choose from given options\n\n");
        printf("- To make a query, enter 1\n");
        printf("- To exit, enter 0\n\n");
        scanf("%d", &choice);
        printf("\n");
        switch (choice){
            case 1:
                printf("Enter the query rectangle in the format: x1 y1 x2 y2: ");
                double x1, y1, x2, y2;
                scanf("%lf %lf %lf %lf", &x1, &y1, &x2, &y2);
                rect queryRect;
                queryRect.maxDim[0] = max(x1, x2);
                queryRect.maxDim[1] = max(y1, y2);
                queryRect.minDim[0] = min(x1, x2);
                queryRect.minDim[1] = min(y1, y2);
                printf("\n");
                searchHRT(hrt, queryRect);
                break;
            case 0:
                break;
        }
    }
    return 0;
}