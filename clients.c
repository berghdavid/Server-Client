#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>

#define PORT 5555
#define buf_size 5
#define data_size 10
#define threads 3

pthread_t tid[threads];
pthread_mutex_t lock;
int buf[buf_size];

void printBuffer(char s[], int buf[])
{
    printf("%s: {", s);
    for(int i = 0; i < buf_size; i++){
        printf("%d ", buf[i]);
    }
    printf("}\n");
}

int fillBuffer(int *buf, int data[], int pointer)
{
    for(int i = 0; i < buf_size; i++)
    {
        *buf = data[pointer + i];
        buf++;
    }
    return 0;
}

void sendData(int clientSocket, int data[])
{
    int buf[buf_size];
    int position = 0;

    fillBuffer(&buf[0], data, position);
    position += buf_size;

    bool ready = true;
    while(ready)
    {
        ready = false;
        send(clientSocket, buf, sizeof(int)*buf_size, 0);
        printBuffer("Data sent", buf);
        recv(clientSocket, buf, sizeof(int)*buf_size, 0);
        printBuffer("Data received", buf);

        if(position + buf_size <= data_size)
        {
            fillBuffer(&buf[0], data, position);
            position += buf_size;
            ready = true;
        }
    }

    printf("Closed connection successfully\n");
}

int connectedSocket()
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    printf("Client socket created successfully...\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("Connected to server successfully...\n");

    return clientSocket;
}

int main()
{
    int data1[data_size] = {7, 5, 4, 1, 9, 8, 9, 7, 5, 4};
    int data2[data_size] = {10, 50, 20, 10, 25, 76, 89, 20, 20, 50};
    int data3[data_size] = {500, 400, 400, 100, 100, 700, 500, 400, 100, 900};

    //if(pthread_mutex_init(&lock, NULL) != 0){
    //    printf("Failed to init mutex\n");
    //    return 1;
    //}

    int clientSocket = connectedSocket();

    sendData(clientSocket, data1);

    return 0;
}