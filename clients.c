#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5555
#define buf_size 5

void connect_client(){
    int clientSocket;
    struct sockaddr_in serverAddr;
    int buf[buf_size] = {7, 5, 4, 1, 9};

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    printf("Client socket created successfully...\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("Connected to server successfully...\n");

    send(clientSocket, buf, sizeof(int)*buf_size, 0);

    printf("Data sent: {");
    for(int i = 0; i < buf_size; i++){
        printf("%d ", buf[i]);
    }
    printf("}\n");
    printf("Closed connection successfully\n");
}

void main(){
    connect_client();
}