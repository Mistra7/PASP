#pragma comment(lib, "Ws2_32.lib")
#define WIN32_LEAN_AND_MEAN

#include "Core.h"
#include "ReceiveThread.h"
#include "Publisher.h"
#include "Subscriber.h"

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
	// socket used to communicate with server
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
				DisplayPosts(&posts);
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

		if (message_length > 0) {
			Send(tp.socket, messageToSend, message_length, &exit);
		}

	}

	//getchar();
	// cleanup
	ShutdownClient(tp.socket, hReceive);

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
		EnterCriticalSection(&exit_cs);
		*exit = true;
		LeaveCriticalSection(&exit_cs);
	}
	else {
		printf("Bytes Sent: %ld\n", iResult);
	}

	ToContinue();
}

void ShutdownClient(SOCKET socket, HANDLE hThread) {
	EnterCriticalSection(&list_cs);
	ClearList(&posts);
	LeaveCriticalSection(&list_cs);
	ClearList(&subbedTopics);

	CloseHandle(hThread);
	DeleteCriticalSection(&list_cs);
	DeleteCriticalSection(&exit_cs);
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