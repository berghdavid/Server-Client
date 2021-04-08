#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define buf_size 5

void printBuffer(int d[]){
    printf("{ ");
    for(int i = 0; i < buf_size; i++){
        printf("%d ", d[i]);
    }
    printf("}\n");
}

int * sortArray(int array[]){
    int *lowest;   // Allocating another int, is this allowed?
    for(int i1 = 0; i1 + 1 < buf_size; i1 ++){
        lowest = &array[i1];
        for(int i2 = i1 + 1; i2 < buf_size; i2 ++){
            if(array[i2] < *lowest){
                lowest = &array[i2];
            }
        }
        int temp = array[i1];
        array[i1] = *lowest;
        *lowest = temp;
    }
    return array;
}

void connect_client(int *q){
    *q += 10;
}

void connect_client2(int *q){
    q = q + 1;
    printf("C FUNC: %p\n", q);
    printf("C FUNC: %d\n", *q);
}

void main(){
    int b[5] = {5, 20, 30, 2, 4};
    printBuffer(sortArray(b));
    //int *pointer = &b[0];
    //printf("b[0]: %p\n", pointer);
    //printf("b[1]: %p\n", pointer + 1);
    //printf("b[0]: %d\n", *pointer);
    //printf("b[1]: %d\n", *(pointer + 1));
    //int a = 10;
    //printf("A before: %d\n", a);
    //connect_client(&a);
    //printf("A after: %d\n", a);

    //int *c = &b[1];
    //printf("C before: %p\n", c);
    //printf("C before: %d\n", *c);
    //connect_client2(c);
    //printf("C after: %p\n", c);
    //printf("C after: %d\n", *c);
}