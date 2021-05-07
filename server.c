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
#include "queue.h"

#define PORT 8751
#define BUF_SIZE 5
#define DATA_SIZE 10
#define SIZE_T 3

int buf[BUF_SIZE];

pthread_mutex_t lock_work = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_work = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_queue = PTHREAD_COND_INITIALIZER;
int active_id = -1;

struct info
{
    int id;
    int server_socket;
    struct queue* q;
};

int * sortArray(int array[])
{
    int *lowest;   // Allocating a pointer, allowed?
    for(int i1 = 0; i1 + 1 < BUF_SIZE; i1 ++) {
        lowest = &array[i1];
        for(int i2 = i1 + 1; i2 < BUF_SIZE; i2 ++) {
            if(array[i2] < *lowest)
                lowest = &array[i2];

        }
        int temp = array[i1]; // Allocating an int, is this allowed?
        array[i1] = *lowest;
        *lowest = temp;
    }
    return array;
}

void printBuffer(char s[], int thr_id, int buf[])
{
    printf("Thread %d ", thr_id);
    printf("%s: { ", s);
    for(int i = 0; i < BUF_SIZE; i++) {
        printf("%d ", buf[i]);
    }
    printf("}\n");
}

int create_socket()
{
    int server_socket;
    struct sockaddr_in server_addr;

    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server_addr, '\0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(server_socket, 
            (struct sockaddr*)&server_addr, 
            sizeof(server_addr)) != 0) {
        printf("Unsuccessfully bound to socket...\n");
        exit(0);
    }

    listen(server_socket, 5);

    return server_socket;
}

void *wait_client(void *socket_desc)
{
    struct info *t_info = (struct info*) socket_desc;
    struct sockaddr_in client_addr;
    socklen_t addr_size;
    
    while(1) {
        int client_socket = accept(t_info->server_socket, 
                            (struct sockaddr*)&client_addr, 
                            &addr_size);
        enq(t_info->q, t_info->id);
        pthread_cond_signal(&cond_queue);

        for(int i = 0; i < DATA_SIZE; i += BUF_SIZE) {
            pthread_mutex_lock(&lock_work);
            while(active_id != t_info->id) {
                pthread_cond_wait(&cond_work, &lock_work);
            }
            
            recv(client_socket, buf, BUF_SIZE*sizeof(int), 0);
            printBuffer("received data", t_info->id, buf);
            send(client_socket, sortArray(buf), sizeof(int)*BUF_SIZE, 0);
            printBuffer("sent data", t_info->id, buf);

            /* If client is sending more packets, enqueue */
            if(i + BUF_SIZE < DATA_SIZE) {
                enq(t_info->q, t_info->id);
            }

            active_id = -1;
            pthread_cond_signal(&cond_queue);
            pthread_mutex_unlock(&lock_work);
        }
    }
}

void handle_threads(struct queue *q)
{
    while(1) {
        pthread_mutex_lock(&lock_work);
        while(queueEmpty(q) || active_id != -1) {
            pthread_cond_wait(&cond_queue, &lock_work);
        }
        active_id = deq(q);

        pthread_cond_broadcast(&cond_work);
        pthread_mutex_unlock(&lock_work);
    }
}

int main()
{
    struct queue *q;
    q = queueCreate();

    int server_socket = create_socket();

    pthread_t thr[SIZE_T];

    /* Initiate working threads */
    for(int i = 0; i < SIZE_T; i++) {
        struct info *d = (struct info *)malloc(sizeof(struct info));
        d->id = i;
        d->server_socket = server_socket;
        d->q = q;

        pthread_create(&thr[i], NULL, (void *)wait_client, (void *)d);
        pthread_detach(thr[i]);
    }
    
    /* Distribute work and signal to other threads */
    while(1) {
        pthread_mutex_lock(&lock_work);
        while(queueEmpty(q) || active_id != -1) {
            pthread_cond_wait(&cond_queue, &lock_work);
        }
        active_id = deq(q);

        pthread_cond_broadcast(&cond_work);
        pthread_mutex_unlock(&lock_work);
    }

    return 0;
}