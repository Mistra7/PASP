#pragma comment(lib, "Ws2_32.lib")
#define WIN32_LEAN_AND_MEAN

#include "Core.h"
#include "Publisher.h"
#include <time.h>

//Port na koji se publisher-i javljaju, za klijente je veci za jedan
#define DEFAULT_PORT 20000

/*
Opis: Slanje poruka na server
Param: Soket servera, poruka koja se salje na server,
duzina poruke i adresa bool-a za gasenje servera
Povratni: Nema
*/
void Send(SOCKET, char*, int, bool*);

/*
Opis: Gasenje klijenta
Param: Soket servera i handle treda za prijem poruka
Povratni: Nema
*/
void ShutdownClient(SOCKET, HANDLE);

static char* rand_string(char* str, size_t size);
static char rand_char();

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

//Za ciscenje stdin buffer-a
int c; 

//Lista postova koji su stigli preko mreze
node* posts;
//Lista topic-a na koje je korinsik pretplacen
node* subbedTopics;

//Kriticne sekcije za rukovanje sa listom post-ova i bool-om za gasenje klijenta
CRITICAL_SECTION list_cs, exit_cs;

int __cdecl main(int argc, char** argv)
{
    // message to send
    char messageToSend[sizeof(article)] = "";
    char text[TEXT_LEN];
	int brClanaka;
	int odabir;

	printf("Odaberi broj clanaka;\n");
	printf("\t1. 500\n");
	printf("\t2. 1000\n");
	printf("\t3. 1500\n");
	scanf_s("%d", &odabir);

	switch (odabir)
	{
	case 1:
		brClanaka = 500;
		break;
	case 2:
		brClanaka = 1000;
		break;
	case 3:
		brClanaka = 1500;
		break;
	default:
		brClanaka = 500;
		break;
	}

    //user_type type;
    u_short port;
    char author[AUTHOR_LEN] = "";
	strcpy_s(author, rand_string(author, AUTHOR_LEN));
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

    port = DEFAULT_PORT;
  

    // create a socket
	SOCKET serverSocket;
	serverSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        ToContinue();
        return 1;
    }
	
	srand(time(0));
    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    inet_pton(serverAddress.sin_family, argv[1], &serverAddress.sin_addr.s_addr);
    serverAddress.sin_port = htons(port);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(serverSocket);
        ToContinue();
        return 1;
    }

	char lit = getchar();
	lit = getchar();

    while (1) {
		for (int i = 0; i < brClanaka; i++)
		{
			int message_length = 0;
			article a;
			a.topic = rand_char();
			strcpy_s(a.authorName, author);
			strcpy_s(a.text, rand_string(a.text, TEXT_LEN));
			//article a = createArticle(author);
			message_length = sizeof(a);
			memcpy(messageToSend, &a, message_length);

			if (message_length > 0) {
				Send(serverSocket, messageToSend, message_length, &exit);
			}
		}
        
		Sleep(100);
    }
    
    //getchar();
    // cleanup
    //ShutdownClient(serverSocket, hReceive);

    return 0;
}

void Send(SOCKET socket, char* messageToSend, int length, bool* exit)
{
    // variable used to store function return value
    int iResult;

    Select(socket, true);

    iResult = send(socket, messageToSend, length, 0);

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        /*EnterCriticalSection(&exit_cs);
        *exit = true;
        LeaveCriticalSection(&exit_cs);*/
    }
    else {
        printf("Bytes Sent: %ld\n", iResult);
    }

    //ToContinue();
}

/*void ShutdownClient(SOCKET socket, HANDLE hThread) {
    EnterCriticalSection(&list_cs);
    ClearList(&posts);
    LeaveCriticalSection(&list_cs);
    ClearList(&subbedTopics);

    CloseHandle(hThread);
    DeleteCriticalSection(&list_cs);
    DeleteCriticalSection(&exit_cs);
    closesocket(socket);
    WSACleanup();
}*/

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

static char* rand_string(char* str, size_t size)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (size) {
		--size;
		for (size_t n = 0; n < size; n++) {
			int key = rand() % (int)(sizeof charset - 1);
			str[n] = charset[key];
		}
		str[size] = '\0';
	}
	return str;
}

static char rand_char()
{
	const char charset[] = "12345";
	int key = rand() % 5;
	return charset[key];
}

