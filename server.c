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
#define SIZE_T 3

int buf[BUF_SIZE];


pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_cond = PTHREAD_MUTEX_INITIALIZER;
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
    for(int i1 = 0; i1 + 1 < BUF_SIZE; i1 ++)
    {
        lowest = &array[i1];
        for(int i2 = i1 + 1; i2 < BUF_SIZE; i2 ++)
        {
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
    for(int i = 0; i < BUF_SIZE; i++)
    {
        printf("%d ", buf[i]);
    }
    printf("}\n");
}

int create_socket()
{
    int server_socket;
    struct sockaddr_in server_addr;

    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    printf("Server socket created successfully...\n");
    memset(&server_addr, '\0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        printf("Unsuccessfully bound to socket...\n");
        exit(0);
    }
    printf("Bound to port number %d successfully...\n", PORT);

    listen(server_socket, 5);
    printf("Listening...\n");

    return server_socket;
}

void *wait_client(void *socket_desc)
{
    struct info *t_info = (struct info*) socket_desc;

    struct sockaddr_in client_addr;
    socklen_t addr_size;
    
    while(1)
    {
        printf("Thread %d waiting for client\n", t_info->id);
        int client_socket = accept(t_info->server_socket, (struct sockaddr*)&client_addr, &addr_size);
        enq(t_info->q, t_info->id);
        printf("Thread %d added to queue\n", t_info->id);
        pthread_cond_broadcast(&cond);

        for(int i = 0; i < 2; i++) {
            pthread_mutex_lock(&lock_cond);
            while(active_id != t_info->id) {
                pthread_cond_wait(&cond, &lock_cond);
            }
            
            recv(client_socket, buf, BUF_SIZE*sizeof(int), 0);
            printBuffer("received data", t_info->id, buf);
            send(client_socket, sortArray(buf), sizeof(int)*BUF_SIZE, 0);

            if(i == 0) {
                enq(t_info->q, t_info->id);
                printf("Thread %d added to queue\n", t_info->id);
            }

            printBuffer("sent data", t_info->id, buf);
            active_id = -1;
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock_cond);
        }
    }
}

void handle_threads(struct queue *q)
{
    while(1)
    {
        pthread_mutex_lock(&lock_cond);
        while(queueEmpty(q) || active_id != -1)
        {
            pthread_cond_wait(&cond, &lock_cond);
            queuePrint(q);
        }
        active_id = deq(q);

        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&lock_cond);
    }
}

int main()
{
    int status;
    int server_socket = create_socket();

    struct queue *q;
    q = queueCreate();

    pthread_t thr[SIZE_T];
    for(int i = 0; i < SIZE_T; i++) {
        struct info *d = (struct info *)malloc(sizeof(struct info));
        d->id = i;
        d->server_socket = server_socket;
        d->q = q;

        pthread_create(&thr[i], NULL, (void *)wait_client, (void *)d);
        pthread_detach(thr[i]);
    }
    
    handle_threads(q);

    return 0;
}