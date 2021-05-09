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

#define PORT 8751
#define BUF_SIZE 5
#define DATA_SIZE 10
#define SIZE_T 3

struct info
{
    int id;
    int *data;
};

void print_buf(char s[], int id, int buf[])
{
    printf("Thread %d %s: { ", id, s);
    for(int i = 0; i < BUF_SIZE; i++) {
        printf("%d ", buf[i]);
    }
    printf("}\n");
}

int fill_buf(int *buf, int data[], int pointer)
{
    for(int i = 0; i < BUF_SIZE; i++) {
        *buf = data[pointer + i];
        buf++;
    }
    return 0;
}

void *send_data(void *socket_desc)
{
    struct info *t_info = (struct info*) socket_desc;
    int client_socket = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(client_socket, (struct sockaddr*)&server_addr, 
    sizeof(server_addr)) != 0) {
        printf("Unsuccessful connection...\n");
        exit(0);
    }

    int buf[BUF_SIZE];

    for(int i = 0; i < DATA_SIZE; i += BUF_SIZE) {
        fill_buf(&buf[0], t_info->data, i);
        send(client_socket, buf, sizeof(int)*BUF_SIZE, 0);
        recv(client_socket, buf, sizeof(int)*BUF_SIZE, 0);
        print_buf("received", t_info->id, buf);
    }

    return 0;
}

int main()
{
    int data[SIZE_T][DATA_SIZE] = {
        {7, 5, 4, 1, 9, 8, 9, 7, 5, 4},
        {10, 50, 20, 10, 25, 76, 89, 20, 20, 50},
        {500, 400, 400, 100, 100, 700, 500, 400, 100, 900}
    };
    
    pthread_t thr[SIZE_T];
    for(int i = 0; i < SIZE_T; i++) {
        struct info *inf = (struct info *)malloc(sizeof(struct info));
        inf->id = i;
        inf->data = &data[i][0];

        pthread_create(&thr[i], NULL, (void *)send_data, (void *)inf);
        pthread_detach(thr[i]);
    }

    sleep(8);

    return 0;
}