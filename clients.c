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
#define data_size 10
#define SIZE_T 3

pthread_mutex_t lock;

struct info
{
    int id;
    int data[data_size];
};

void print_buf(char s[], int buf[])
{
    printf("%s: {", s);
    for(int i = 0; i < BUF_SIZE; i++)
    {
        printf("%d ", buf[i]);
    }
    printf("}\n");
}

int fill_buf(int *buf, int data[], int pointer)
{
    for(int i = 0; i < BUF_SIZE; i++)
    {
        *buf = data[pointer + i];
        buf++;
    }
    return 0;
}

int send_data(int id, int data[])
{
    int client_socket = socket(PF_INET, SOCK_STREAM, 0);
    printf("Client socket created successfully...\n");

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(client_socket, 
    (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        printf("Unsuccessful connection...\n");
        exit(0);
    }
    printf("Thread %d connected to server successfully...\n", id);

    int buf[BUF_SIZE];
    int position = 0;

    fill_buf(&buf[0], data, position);
    position += BUF_SIZE;

    bool ready = true;
    while(ready)
    {
        ready = false;
        send(client_socket, buf, sizeof(int)*BUF_SIZE, 0);
        print_buf("Data sent", buf);
        recv(client_socket, buf, sizeof(int)*BUF_SIZE, 0);
        print_buf("Data received", buf);

        if(position + BUF_SIZE <= data_size)
        {
            fill_buf(&buf[0], data, position);
            position += BUF_SIZE;
            ready = true;
        }
    }

    printf("Closed connection successfully\n");
    return 0;
}

int main()
{
    int status;

    int data[3][data_size] = {
        {7, 5, 4, 1, 9, 8, 9, 7, 5, 4},
        {10, 50, 20, 10, 25, 76, 89, 20, 20, 50},
        {500, 400, 400, 100, 100, 700, 500, 400, 100, 900}
    };

    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to init mutex\n");
        return 1;
    }
    
    for(int i = 0; i < SIZE_T; i++) {
        send_data(i, data[i]);
    }

    /*
    pthread_t thr[SIZE_T];
    for(int i = 0; i < SIZE_T; i++) {
        struct info *inf = (struct info *)malloc(sizeof(struct info));
        inf->id = i;
        inf->data = d;
        for(int j = 0; j < data_size; j++) {
            inf->data[j] = data[3][j];
        }

        pthread_create(&thr[i], NULL, (void *)send_data, (void *)inf);
        pthread_detach(thr[i]);
    }*/

    //sleep(8);

    return 0;
}