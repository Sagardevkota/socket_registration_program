#include<stdio.h>
#include<winsock2.h>
#include <thread>
#include <map>

#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define DEFAULT_BUFLEN 512

int numOfThread = 0;

void *handleClients(void *clientSocket);

void sendMessage(char *message, const SOCKET *socket);


std::string findInfoFromFile(char *rollNum);

int main(int argc, char *argv[]) {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;

    //roll number via command line argument
    char *portText = argv[1];
    int portNum;
    sscanf(argv[2], "%d", &portNum);

    if (strcmp(portText, "-p") != 0) {
        printf("type -p for port number");
        return 1;
    }
    if (portNum > 65535 || portNum < 1024) {
        printf("port number not valid");
        return 1;
    }


    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");

    //Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
    }

    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(portNum);

    //Bind
    if (bind(server_socket, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
    }

    puts("Bind done");


    //Listen to incoming connections
    listen(server_socket, 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");

    int c = sizeof(struct sockaddr_in);

    //accept method gives the socket of client
    while ((client_socket = accept(server_socket, (struct sockaddr *) &client, &c)) != INVALID_SOCKET) {

        if (numOfThread > 10) {
            printf("Number of client thread exceeded");
            sendMessage("Number of client thread exceeded", &client_socket);
            closesocket(client_socket);
            continue;
        }

        puts("New Connection accepted");


        pthread_t thread_id;

        if (pthread_create(&thread_id, NULL, handleClients, (void *) &client_socket) != 0) {
            perror("could not create thread");
            return 1;
        }

        numOfThread++;


    }


    closesocket(server_socket);
    WSACleanup();

    return 0;

}

void *handleClients(void *clientSocket) {

    SOCKET client_socket = *(SOCKET *) clientSocket;

    while (true) {
        char recvbuf[DEFAULT_BUFLEN];

        int recvbuflen = DEFAULT_BUFLEN;
        int iResult = recv(client_socket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[recvbuflen] = '\0';
            if (strcmp(recvbuf, "quit") == 0) {
                printf("client wants to quit \n");
                closesocket(client_socket);
                continue;

            }

//            char *recvbuf = receiveMessage(&client_socket);
            printf("Roll number %s received \n", recvbuf);
            std::string info = findInfoFromFile(recvbuf);
            char *userInfo = &info[0];
            sendMessage(userInfo, &client_socket);
        }

        //clear the message buffer
        memset(recvbuf, 0, recvbuflen);


    }
}


std::string findInfoFromFile(char *rollNum) {
    int lineNum = (int) *rollNum % 11;

    printf("Reading from line number %d \n", lineNum);

    char c[10];
    FILE *fptr;
    if ((fptr = fopen("D:\\ClionProjects\\socket-registration-program\\users.txt", "r")) == nullptr) {
        printf("Error! File cannot be opened.");
        // Program exits if the file pointer returns NULL.
        exit(1);
    }

    int i = 0;
    std::string name;
    while (fgets(c, sizeof(c), fptr)) {
        i++;
        if (i == lineNum) {
            name = c;
        }
    }

    fclose(fptr);
    std::string server_reply;
    if (name.length()!=0)
     server_reply = "Welcome <<" + name + ">>, your identification key is <<" + rollNum + ".>>";
    else
        server_reply = "Sorry, your identification key was not found in the file";

    return server_reply;

}

void sendMessage(char *message, const SOCKET *client_socket) {

    int iResult = send(*client_socket, message, (int) strlen(message), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(*client_socket);
        WSACleanup();

    }

}







