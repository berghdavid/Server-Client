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

#define PORT 8751       // Port for communication, use any available port
#define BUF_SIZE 5      // Size of buffer array
#define DATA_SIZE 10    // Total size of client array
#define T_SIZE 3        // Number of threads handling clients (N)
#define RECEIVED "received"
#define SENT "sent"

/* Used to store all necessary data with a pthread */
struct info
{
    int id;
    int *data;
    struct sockaddr_in server_addr;
};

struct sockaddr_in get_server_addr()
{
    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    return server_addr;
}

/** Prints the received/sent array of ints.
 * 
 * @param s String which is either "received" or "sent".
 * @param thr_id Int ID of thread.
 * @param buf Array of ints to be printed.
*/
void print_buf(char s[], int id, int buf[])
{
    printf("Thread %d %s: { ", id, s);
    for(int i = 0; i < BUF_SIZE; i++) {
        printf("%d ", buf[i]);
    }
    printf("}\n");
}

/**
 * Used to fill an array with integers before sending.
 * 
 * @param buf Array to be filled.
 * @param data Array to copy from. Size of array must
 * be greater than the given 'index' + 'BUF_SIZE'
 * @param index Determines the starting index in *buf 
 * from which the integers will be copied from.
 */
int fill_buf(int *buf, int data[], int index)
{
    for(int i = 0; i < BUF_SIZE; i++) {
        *buf = data[index + i];
        buf++;
    }
    return 0;
}

void *send_data(void *socket_desc)
{
    struct info *t_info = (struct info*) socket_desc;
    int client_socket = socket(PF_INET, SOCK_STREAM, 0);

    if(connect(client_socket, 
            (struct sockaddr*)&t_info->server_addr, 
            sizeof(t_info->server_addr)) != 0) {
        printf("Unsuccessful connection...\n");
        exit(0);
    }

    int buf[BUF_SIZE];

    for(int i = 0; i < DATA_SIZE; i += BUF_SIZE) {
        fill_buf(&buf[0], t_info->data, i);

        send(client_socket, buf, sizeof(int)*BUF_SIZE, 0);
        print_buf(SENT, t_info->id, buf);
        recv(client_socket, buf, sizeof(int)*BUF_SIZE, 0);
        print_buf(RECEIVED, t_info->id, buf);
    }

    return 0;
}

int main()
{
    int data[T_SIZE][DATA_SIZE] = {
        {7, 5, 4, 1, 9, 8, 9, 7, 5, 4},
        {10, 50, 20, 10, 25, 76, 89, 20, 20, 50},
        {500, 400, 400, 100, 100, 700, 500, 400, 100, 900}
    };

    struct sockaddr_in server_addr = get_server_addr();
    
    /* Start 'T_SIZE' different client threads */
    pthread_t thr[T_SIZE];
    for(int i = 0; i < T_SIZE; i++) {
        struct info *inf = (struct info *)malloc(sizeof(struct info));
        inf->id = i;
        inf->data = &data[i][0];
        inf->server_addr = server_addr;

        pthread_create(&thr[i], NULL, (void *)send_data, (void *)inf);
        //sleep(1); // Uncomment when testing queue orders
    }

    /* Wait for all threads, then join them when finished */
    for(int i = 0; i < T_SIZE; i++) {
        if(pthread_join(thr[i], NULL) != 0)
            printf("Could not join with thread %d\n", i);
    }

    return 0;
}