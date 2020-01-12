#pragma comment(lib, "Ws2_32.lib")
#include "PublisherThread.h"
#include "SubscriberThread.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "20000"
#define DEFAULT_PORT2 "20001"
#define SERVER_SLEEP_TIME 50

bool InitializeWindowsSockets();

int main()
{
	// Socket used for listening for new clients 
	//soket za slusanje pablisera
	SOCKET listenForPublishersSocket = INVALID_SOCKET;
	//soket za slusanje sabskrajbera
	SOCKET listenForSubscribersSocket = INVALID_SOCKET;
	// Socket used for communication with client
	SOCKET acceptedSocket = INVALID_SOCKET;
	// variable used to store function return value
	int iResult;
	// Buffer used for storing incoming data
	char recvbuf[DEFAULT_BUFLEN];

	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}

	// Prepare address information structures
	addrinfo* resultingAddress = NULL;
	addrinfo* resultingAddress2 = NULL;
	addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4 address
	hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
	hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
	hints.ai_flags = AI_PASSIVE;     // 

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	listenForPublishersSocket = socket(AF_INET,      // IPv4 address famly
		SOCK_STREAM,  // stream socket
		IPPROTO_TCP); // TCP

	if (listenForPublishersSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket - bind port number and local address 
	// to socket
	iResult = bind(listenForPublishersSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		closesocket(listenForPublishersSocket);
		WSACleanup();
		return 1;
	}

	// Since we don't need resultingAddress any more, free it
	freeaddrinfo(resultingAddress);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4 address
	hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
	hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
	hints.ai_flags = AI_PASSIVE;     // 

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT2, &hints, &resultingAddress2);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	listenForSubscribersSocket = socket(AF_INET,      // IPv4 address famly
		SOCK_STREAM,  // stream socket
		IPPROTO_TCP); // TCP

	if (listenForSubscribersSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(resultingAddress2);
		WSACleanup();
		return 1;
	}

	iResult = bind(listenForSubscribersSocket, resultingAddress2->ai_addr, (int)resultingAddress2->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(resultingAddress2);
		closesocket(listenForPublishersSocket);
		WSACleanup();
		return 1;
	}

	// Since we don't need resultingAddress any more, free it
	freeaddrinfo(resultingAddress2);

	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(listenForPublishersSocket, FIONBIO, &nonBlockingMode);

	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", WSAGetLastError());
		return -1;
	}

	iResult = ioctlsocket(listenForSubscribersSocket, FIONBIO, &nonBlockingMode);

	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", WSAGetLastError());
		return -1;
	}

	printf("Server initialized, waiting for clients.\n");

	// Set listenSocket in listening mode
	iResult = listen(listenForPublishersSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenForPublishersSocket);
		WSACleanup();
		return 1;
	}

	// Set listenSocket in listening mode
	iResult = listen(listenForSubscribersSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenForSubscribersSocket);
		WSACleanup();
		return 1;
	}

	FD_SET set;
	timeval timeVal;

	do
	{
		timeVal.tv_sec = 0;
		timeVal.tv_usec = 0;
		FD_ZERO(&set);
		FD_SET(listenForPublishersSocket, &set);
		iResult = select(0, &set, NULL, NULL, &timeVal);
			//listen(listenForPublishersSocket, SOMAXCONN);
		if (iResult > 0)
		{
			acceptedSocket = accept(listenForPublishersSocket, NULL, NULL);

			if (acceptedSocket == INVALID_SOCKET)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(listenForPublishersSocket);
				WSACleanup();
			}
			else
			{
				//addSocket(&publisherRoot, acceptedSocket);
			}
		}

		timeVal.tv_sec = 0;
		timeVal.tv_usec = 0;

		FD_ZERO(&set);
		FD_SET(listenForSubscribersSocket, &set);
		iResult = select(0, &set, NULL, NULL, &timeVal);

		if (iResult > 0)
		{
			acceptedSocket = accept(listenForSubscribersSocket, NULL, NULL);

			if (acceptedSocket == INVALID_SOCKET)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(listenForSubscribersSocket);
				WSACleanup();
			}
			else
			{
				while (true)
				{
					FD_ZERO(&set);
					FD_SET(acceptedSocket, &set);

					timeVal.tv_sec = 0;
					timeVal.tv_usec = 0;

					iResult = select(0, &set, NULL, NULL, &timeVal);
					if (iResult == SOCKET_ERROR)
					{
						printf("Select failed with error: %d", WSAGetLastError());
						break;
					}

					if (iResult == 0)
					{
						Sleep(10);
						continue;
					}

					iResult = recv(acceptedSocket, recvbuf, DEFAULT_BUFLEN, 0);
					if (iResult > 0)
					{
						add(recvbuf[0], acceptedSocket);
						break;
					}
				}
				
			}
		}

	} while (1);

	// shutdown the connection since we're done
	iResult = shutdown(acceptedSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(acceptedSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(listenForPublishersSocket);
	closesocket(acceptedSocket);
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