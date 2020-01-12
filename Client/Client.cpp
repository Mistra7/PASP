#pragma comment(lib, "Ws2_32.lib")
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 20000
#define SERVER_SLEEP_TIME 50
#define TEXT_LEN 100

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

int __cdecl main(int argc, char** argv)
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // message to send
    char messageToSend[DEFAULT_BUFLEN] = "";
    //char text[TEXT_LEN];
    int user_type;
    u_short port;

    int c; // for clearing the stdin buffer

    // Validate the parameters
    if (argc != 2)
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    if (InitializeWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

    do {
        system("cls");
        printf("0 - PUBLISHER\t1 - SUBSCRIBER\n");
        printf("Choose user type: ");
        scanf_s("%d", &user_type);
        while ((c = getchar()) != '\n' && c != EOF) {}
    } while (user_type != 0 && user_type != 1);

    if (user_type == 0) {
        port = DEFAULT_PORT;
    }
    else {
        port = DEFAULT_PORT + 1;
    }
    
    // create a socket
    connectSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    inet_pton(serverAddress.sin_family, argv[1], &serverAddress.sin_addr.s_addr);
    serverAddress.sin_port = htons(DEFAULT_PORT);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
    }

    while (1) {
        //Select(connectSocket);
        // Send an prepared message with null terminator included
        printf("Enter message to send: ");
        scanf_s("%[^\n]", messageToSend, DEFAULT_BUFLEN);
        while ((c = getchar()) != '\n' && c != EOF) {}

        /*messageToSend = (char*)malloc(strlen(text) + 1);
        memcpy(messageToSend, text, strlen(text) + 1);*/
        if (strcmp(messageToSend, "STOP") == 0) {
            break;
        }

        iResult = send(connectSocket, messageToSend, (int)strlen(messageToSend) + 1, 0);

        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }

        printf("Bytes Sent: %ld\n", iResult);
    }
    
    //getchar();
    // cleanup
    closesocket(connectSocket);
    WSACleanup();

    return 0;
}

bool InitializeWindowsSockets()
{
    WSADATA wsaData;
    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
    return true;
}

void Select(SOCKET socket) {
    // Initialize select parameters
    FD_SET set;
    timeval timeVal;

    // Set timeouts to zero since we want select to return
    // instantaneously
    timeVal.tv_sec = 0;
    timeVal.tv_usec = 0;

    int iResult;

    do {
        FD_ZERO(&set);
        // Add socket we will wait to read from
        FD_SET(socket, &set);

        iResult = select(0, NULL, &set, NULL, &timeVal);

        // lets check if there was an error during select
        if (iResult == SOCKET_ERROR)
        {
            fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
            continue;
        }

        // now, lets check if there are any sockets ready
        if (iResult == 0)
        {
            // there are no ready sockets, sleep for a while and check again
            Sleep(SERVER_SLEEP_TIME);
            continue;
        }
    } while (iResult != 1);
}