#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5555
#define buf_size 5

void main(){
    int sockfd;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;
    int buf[buf_size];

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    printf("Server socket created successfully...\n");
    memset(&serverAddr, '\0', sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("Bound to port number %d successfully...\n", PORT);

    listen(sockfd,6);
    printf("Listening...\n");
    newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);

    recv(newSocket, buf, sizeof(int)*buf_size, 0);

    printf("Data received: { ");
    for(int i = 0; i < buf_size; i++){
        printf("%d ", buf[i]);
    }
    printf("}\n");

    printf("Closed connection successfully\n");
}

// TO RUN:
// gcc server.c -o server
// ./server

// gcc clients.c -o clients
// ./clients