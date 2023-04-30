#include <stdio.h>
#include "hilbert_r_tree.c"

int main(){
    FILE* fp = fopen("test.txt", "r");
    char buffer[1024];
    while(fgets(buffer, BUFFERSIZE, fp)!=NULL){
        
    }
}