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

#define PORT 8751       // Port for communication, use any available port
#define BUF_SIZE 5      // Size of buffer array
#define DATA_SIZE 10    // Total size of client array
#define T_SIZE 3        // Number of threads handling clients (N)
#define BACKLOG 5       // Max amount of pending connections for 'listen()'
#define RECEIVED "received"
#define SENT "sent"

pthread_mutex_t lock_work = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_work = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_queue = PTHREAD_COND_INITIALIZER;

int buf[BUF_SIZE];
int active_id = -1;

/** Stores all necessary data for a pthread. */
struct info
{
    int id;
    int server_socket;
    struct queue* q;
};

/**
 * Sorts and returns an array using insertion sort. 
 * Insertion sort is fast and easy for small arrays.
 */
int * sort_array()
{
    int key, i, j;

    for(i = 1; i < BUF_SIZE; i++) {
        key = buf[i];
        j = i - 1;

        while(j >= 0 && buf[j] > key) {
            buf[j + 1] = buf[j];
            j = j - 1;
        }
        buf[j + 1] = key;
    }
    return buf;
}

/** Prints the received/sent array of ints.
 * 
 * @param s String which is either "received" or "sent".
 * @param thr_id Int ID of thread.
 * @param buf Array of ints to be printed.
*/
void print_buf(char s[], int thr_id, int buf[])
{
    printf("Thread %d ", thr_id);
    printf("%s data: { ", s);
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

    listen(server_socket, BACKLOG);

    return server_socket;
}

/** 
 * Waits for a client to connect to, then communicates
 * with the connected client.
 */
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


        /* Iterate through all packets of arrays to be received */
        for(int i = 0; i < DATA_SIZE; i += BUF_SIZE) {
            //sleep(3); // Uncomment when testing queue orders

            pthread_mutex_lock(&lock_work);
            while(active_id != t_info->id) {
                /* Wait for main thread to broadcast */
                pthread_cond_wait(&cond_work, &lock_work); // 
            }
            
            recv(client_socket, buf, BUF_SIZE*sizeof(int), 0);
            print_buf(RECEIVED, t_info->id, buf);
            send(client_socket, sort_array(), sizeof(int)*BUF_SIZE, 0);
            print_buf(SENT, t_info->id, buf);

            /* If client has more packets to send, enqueue */
            if(i < DATA_SIZE - BUF_SIZE) {
                enq(t_info->q, t_info->id);
            }

            /* Unlock and signal the main thread */
            active_id = -1;
            pthread_cond_signal(&cond_queue);
            pthread_mutex_unlock(&lock_work);
        }
    }
}

int main()
{
    int server_socket = create_socket();

    /* Build FIFO queue for threads with work */
    struct queue *q;
    q = queueCreate();

    /* Start threads which handle clients */
    pthread_t thr[T_SIZE];
    for(int i = 0; i < T_SIZE; i++) {
        struct info *d = (struct info *)malloc(sizeof(struct info));
        d->id = i;
        d->server_socket = server_socket;
        d->q = q;

        pthread_create(&thr[i], NULL, (void *)wait_client, (void *)d);
        pthread_detach(thr[i]);
    }

    printf("Listening for clients...\n");
    
    /* Distribute work by signalling the threads in queue */
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