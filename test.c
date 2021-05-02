#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define buf_size 5
#define SIZE_T 3

pthread_mutex_t lock;

void do_something(){
    for(int i = 0; i < 2; i++)
    {
        pthread_mutex_lock(&lock);
        printf("Thread %lu doing something\n", pthread_self());
        sleep(2);
        pthread_mutex_unlock(&lock);
    }
    
}

int main(){
    if(pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("Mutex init failed\n");
        return 1;
    }
    pthread_t thr[SIZE_T];

    for(int i = 0; i < SIZE_T; i++)
    {
        pthread_create(&thr[i], NULL, (void *)do_something, NULL);
        
        pthread_detach(thr[i]);
    }

    sleep(15);

    return 1;
}