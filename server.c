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

pthread_mutex_t lock;
int buf[buf_size];

int * sortArray(int array[])
{
    int *lowest;   // Allocating a pointer, allowed?
    for(int i1 = 0; i1 + 1 < buf_size; i1 ++)
    {
        lowest = &array[i1];
        for(int i2 = i1 + 1; i2 < buf_size; i2 ++)
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

void printBuffer(char s[])
{
    printf("%s: { ", s);
    for(int i = 0; i < buf_size; i++){
        printf("%d ", buf[i]);
    }
    printf("}\n");
}

int main()
{
    int sockfd;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    printf("Server socket created successfully...\n");
    memset(&serverAddr, '\0', sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("Bound to port number %d successfully...\n", PORT);

    listen(sockfd, 5);
    printf("Listening...\n");
    newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
    // Mutex Lock

    // TODO: Fix communication to signal EOF from clients
    for(int i = 0; i < 2; i++)
    {
        recv(newSocket, buf, sizeof(int)*buf_size, 0);
        printBuffer("Server: Data received");
        send(newSocket, sortArray(buf), sizeof(int)*buf_size, 0);
        printBuffer("Server: Data sent");
    }
    // Mutex Unlock
    
    printf("Closed connection successfully\n");

    return 0;
}