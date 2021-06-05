//
// Created by hp on 6/5/2021.
//


#include<stdio.h>
#include<winsock2.h>


#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define DEFAULT_BUFLEN 512

void getInfo(char *rollNum, const SOCKET *client_socket);

bool doesUserWantMore = true;

void sendMessage(char *message, const SOCKET *client_socket);


int main(int argc, char *argv[]) {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;


    char *serverAddress = argv[2];
    char *portText = argv[3];
    char *nText = argv[5];
    int portNum;
    sscanf(argv[4], "%d", &portNum);

    if (strcmp(portText, "-p") != 0) {
        printf("type -p for port number");
        return 1;
    }
    if (portNum > 65535 || portNum < 1024) {
        printf("port number not valid");
        return 1;
    }
    if (strcmp(nText, "-n") != 0) {
        printf("type -n for denoting idnum in front of id");
        return 1;
    }


    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");

    //Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
    }

    printf("Socket created.\n");


    server.sin_addr.s_addr = inet_addr(serverAddress);
    server.sin_family = AF_INET;
    server.sin_port = htons(portNum);

    //Connect to remote server
    if (connect(client_socket, (struct sockaddr *) &server, sizeof(server)) < 0) {
        puts("connect error");
        return 1;
    }

    puts("Connected");


    char *rollNum = argv[6];

    getInfo(rollNum, &client_socket);

    while (doesUserWantMore) {
        printf("Enter your roll number again");

        char roll[DEFAULT_BUFLEN];
        scanf("%s", roll);
        getInfo(roll, &client_socket);
    }
    return 0;
}

void getInfo(char *rollNum, const SOCKET *client_socket) {

    //send roll number
    int iResult = send(*client_socket, rollNum, (int) strlen(rollNum), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(*client_socket);
        WSACleanup();

    }

    puts("Connected to server waiting for result");

    char recvbuf[DEFAULT_BUFLEN];

    int recvbuflen = DEFAULT_BUFLEN;

    iResult = recv(*client_socket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        recvbuf[recvbuflen] = '\0';

        printf("%s \n", recvbuf);

        if (strcmp(recvbuf, "Number of client thread exceeded") == 0) {
            closesocket(*client_socket);
            doesUserWantMore = false;
            return;
        }

        printf("Do you want to see info again?YES/NO");
        //clear the message buffer
        memset(recvbuf, 0, recvbuflen);

        char decision[20];
        scanf("%s", decision);
        if (strcmp(decision, "YES") == 0)
            doesUserWantMore = true;
        else {
            doesUserWantMore = false;
            sendMessage("quit", client_socket);
        }


    }

}

void sendMessage(char *message, const SOCKET *client_socket) {

    int iResult = send(*client_socket, message, (int) strlen(message), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(*client_socket);
        WSACleanup();

    }

}