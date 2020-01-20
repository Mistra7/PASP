#pragma comment(lib, "Ws2_32.lib")
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include "ReceiveThread.h"
#include "Publisher.h"

#define DEFAULT_PORT 20000

void ShutdownClient(SOCKET, HANDLE);

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

void SendMessage(SOCKET, char*, int, HANDLE);

int SubscriberMenu();

void SubToTopic(char*, int*);

bool isSubbed(char, int);

void DisplayPosts();

void printArticle(article);

void ToContinue();

int c; // za ciscenje stdin buffer-a

node* subbedTopics = NULL; // teme na koje je korisnik pretplacen (ne koristi se za publisher-e)
node* posts = NULL;

CRITICAL_SECTION list_cs, exit_cs;

int __cdecl main(int argc, char** argv)
{
    // socket used to communicate with server
    //SOCKET connectSocket = 
    // message to send
    char messageToSend[DEFAULT_BUFLEN] = "";
    char text[TEXT_LEN];

    //user_type type;
    u_short port;
    char author[AUTHOR_LEN] = "";

    DWORD receiveID;
    HANDLE hReceive;

    bool exit = false;

    threadParam tp;
    tp.exit = &exit;
    tp.socket = INVALID_SOCKET;

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
        scanf_s("%d", &tp.type);
        while ((c = getchar()) != '\n' && c != EOF) {}
    } while (tp.type != 0 && tp.type != 1);

    if (tp.type == PUBLISHER) {
        port = DEFAULT_PORT;
    }
    else {
        port = DEFAULT_PORT + 1;
    }

    // create a socket
    tp.socket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (tp.socket == INVALID_SOCKET)
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
    if (connect(tp.socket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(tp.socket);
        ToContinue();
        return 1;
    }


    hReceive = CreateThread(NULL, 0, &receive, &tp, 0, &receiveID);
    InitializeCriticalSection(&list_cs);
    InitializeCriticalSection(&exit_cs);

    if (tp.type == PUBLISHER) {
        system("cls");
        printf("Enter username: ");
        scanf_s("%[^\n]", author, AUTHOR_LEN);
        while ((c = getchar()) != '\n' && c != EOF) {}
    }

    while (1) {
        int message_length = 0;

        if (tp.type == PUBLISHER) {
            switch (publisherMenu())
            {
            case 1: if (!exit) {
                    article a = createArticle(author);
                    message_length = sizeof(a);
                    memcpy(messageToSend, &a, message_length);
                }
                break;
            case 2: EnterCriticalSection(&exit_cs);
                exit = true;
                LeaveCriticalSection(&exit_cs);
                break;
            default:
                break;
            }
        }
        else if (tp.type == SUBSCRIBER) {
            switch (SubscriberMenu())
            {
            case 1: if (!exit) {
                    SubToTopic(messageToSend, &message_length);
                }
                break;
            case 2: EnterCriticalSection(&list_cs);
                DisplayPosts();
                LeaveCriticalSection(&list_cs);
                break;
            case 3: EnterCriticalSection(&exit_cs);
                exit = true;
                LeaveCriticalSection(&exit_cs);
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
            SendMessage(tp.socket, messageToSend, message_length, hReceive);
        }
        
    }
    
    //getchar();
    // cleanup
    ShutdownClient(tp.socket, hReceive);

    return 0;
}

void ShutdownClient(SOCKET socket, HANDLE hThread) {
    EnterCriticalSection(&list_cs);
    ClearList(&posts);
    LeaveCriticalSection(&list_cs);
    ClearList(&subbedTopics);

    CloseHandle(hThread);
    DeleteCriticalSection(&list_cs);
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
        topic = chooseTopic();

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

void DisplayPosts() {
    int postCnt = Count(posts);
    system("cls");

    if (postCnt == 0) {
        printf("No new posts have been received\n");
    }
    else {
        for (int i = 0; i < postCnt; i++)
        {
            printArticle(*(article*)ElementAt(posts, i));
            printf("---------------------------------------------");
        }
        ClearList(&posts);
    }
    ToContinue();
}

void printArticle(article post) {
    switch (post.topic)
    {
    case ('1'): printf("GAMING\n");
        break;
    case ('2'): printf("TECHNOLOGY\n");
        break;
    case ('3'): printf("MEMES\n");
        break;
    case ('4'): printf("CELEBRITIES\n");
        break;
    case ('5'): printf("SPORT\n");
        break;
    default:
        break;
    }

    printf("%s\n", post.text);
    printf("by %s\n", post.authorName);
}