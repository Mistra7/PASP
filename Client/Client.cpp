#pragma comment(lib, "Ws2_32.lib")
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include "List.h"
#include "ReceiveThread.h"

#define DEFAULT_PORT 20000
#define TEXT_LEN 100

enum user_type{PUBLISHER, SUBSCRIBER};

void ShutdownClient(SOCKET, HANDLE);

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

void SendMessage(SOCKET, char*, int, HANDLE);

int SubscriberMenu();

void SubToTopic(char*, int*);

bool isSubbed(char, int);

void ToContinue();

int c; // za ciscenje stdin buffer-a

node* subbedTopics = NULL; // teme na koje je korisnik pretplacen (ne koristi se za publisher-e)

CRITICAL_SECTION cs;
HANDLE hEnableReceive;

int __cdecl main(int argc, char** argv)
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    // message to send
    char messageToSend[DEFAULT_BUFLEN] = "";
    //char text[TEXT_LEN];
    user_type type;
    u_short port;

    DWORD receiveID;
    HANDLE hReceive;

    bool exit = false;

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
        scanf_s("%d", &type);
        while ((c = getchar()) != '\n' && c != EOF) {}
    } while (type != 0 && type != 1);

    if (type == PUBLISHER) {
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
        ToContinue();
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    inet_pton(serverAddress.sin_family, argv[1], &serverAddress.sin_addr.s_addr);
    serverAddress.sin_port = htons(port);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        ToContinue();
        return 1;
    }


    hReceive = CreateThread(NULL, 0, &receive, &connectSocket, 0, &receiveID);
    hEnableReceive = CreateSemaphore(NULL, 0, 1, NULL);
    InitializeCriticalSection(&cs);

    if (type == SUBSCRIBER) {
        ReleaseSemaphore(hEnableReceive, 1, NULL);
    }

    while (1) {
        int message_length = 0;

        if (type == PUBLISHER) {

        }
        else if (type == SUBSCRIBER) {
            switch (SubscriberMenu())
            {
            case 1: SubToTopic(messageToSend, &message_length);
                break;
            case 2:
                break;
            case 3: exit = true;
                break;
            default:
                break;
            }
        }

        if (exit) {
            break;
        }
       
        // Send an prepared message with null terminator included
        /*printf("Enter message to send: ");
        scanf_s("%[^\n]", messageToSend, DEFAULT_BUFLEN);
        while ((c = getchar()) != '\n' && c != EOF) {}*/

        /*messageToSend = (char*)malloc(strlen(text) + 1);
        memcpy(messageToSend, text, strlen(text) + 1);*/
        /*if (strcmp(messageToSend, "STOP") == 0) {
            break;
        }*/
        if (message_length > 0) {
            SendMessage(connectSocket, messageToSend, message_length, hReceive);
        }
        
    }
    
    //getchar();
    // cleanup
    ShutdownClient(connectSocket, hReceive);

    return 0;
}

void ShutdownClient(SOCKET socket, HANDLE hThread) {
    ClearList(&subbedTopics);
    CloseHandle(hThread);
    CloseHandle(hEnableReceive);
    DeleteCriticalSection(&cs);
    closesocket(socket);
    WSACleanup();
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

void SendMessage(SOCKET socket, char* messageToSend, int length, HANDLE hThread)
{
    // variable used to store function return value
    int iResult;

    Select(socket, true);

    iResult = send(socket, messageToSend, length, 0);

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        ShutdownClient(socket, hThread);
        ToContinue();
        exit(1);
    }

    printf("Bytes Sent: %ld\n", iResult);
    ToContinue();
}

int SubscriberMenu() {
    int option;

    do {
        system("cls");
        printf("1. Subscribe to a topic\n");
        printf("2. Read posts\n");
        printf("3. Exit\n");

        printf("Choose an option: ");
        scanf_s("%d", &option);
        while ((c = getchar()) != '\n' && c != EOF) {}
    } while (option < 1 || option > 3);

    return option;
}

void SubToTopic(char* messageToSend, int* length) {
    char topic;
    int topics_subbed = Count(subbedTopics);
    if (topics_subbed != 5) {
        do {
            system("cls");
            printf("1 - GAMING\t");
            printf("2 - TECHNOLOGY\t");
            printf("3 - MEMES\t");
            printf("4 - CELEBRITIES\t");
            printf("5 - SPORT\n");

            printf("Choose a topic: ");
            topic = getchar();
            while ((c = getchar()) != '\n' && c != EOF) {}
        } while (topic < '1' || topic > '5');

        if (isSubbed(topic, topics_subbed)) {
            printf("Already subscribed to topic\n");
            ToContinue();
        }
        else {
            AddToList(&subbedTopics, &topic, sizeof(topic));
            messageToSend[(*length)++] = topic;
        }
    }
    else
    {
        system("cls");
        printf("Subscribed to all topics\n");
        ToContinue();
    }
    
}

void ToContinue() {
    printf("\nPress ENTER key to continue...");
    c = getchar();
}

bool isSubbed(char topic, int topics_subbed) {
    bool subbed = false;

    if (topics_subbed == 5) {
        subbed = true;
    }
    else {
        for (int i = 0; i < topics_subbed; i++){
            if (topic == *(char*)ElementAt(subbedTopics, i)) {
                subbed = true;
            }
        }
    }

    return subbed;
}