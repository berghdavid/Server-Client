#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 5555
#define buf_size 5
#define data_size 10

int buf[buf_size];

void printBuffer(char s[]){
    printf("%s: {", s);
    for(int i = 0; i < buf_size; i++){
        printf("%d ", buf[i]);
    }
    printf("}\n");
}

void fillBuffer(int buf[], int data[], int pointer){
    for(int i = 0; i < buf_size; i++){
        buf[i] = data[pointer + i];
    }
}

void connect_client(){
    int clientSocket;
    struct sockaddr_in serverAddr;

    int data[data_size] = {7, 5, 4, 1, 9, 5, 4, 3, 2, 1};

    int position = 0;
    fillBuffer(buf, data, position);
    position += buf_size;

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    printf("Client socket created successfully...\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("Connected to server successfully...\n");

    bool ready = true;
    while(ready){
        ready = false;
        send(clientSocket, buf, sizeof(int)*buf_size, 0);
        printBuffer("Data sent");
        recv(clientSocket, buf, sizeof(int)*buf_size, 0);
        printBuffer("Data received");

        if(position < data_size){
            fillBuffer(buf, data, position);
            position += buf_size;
            ready = true;
        }
    }

    printf("Closed connection successfully\n");
}

void main(){
    connect_client();
}