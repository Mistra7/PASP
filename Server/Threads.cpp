#include "Threads.h"

SocketNode* publisherRoot = NULL;
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "20000"
#define DEFAULT_PORT2 "20001"

DWORD WINAPI helpPublishers(LPVOID lpParam)
{
	//buffer u koje smjestamo poruke klijenata
	char recvbuf[DEFAULT_BUFLEN];
	FD_SET set;
	timeval timeVal;
	//za smjestanje povratnih vrijednosti funkcija
	int iResult;
	//clanak koji dobijamo od klijenta
	struct article recvArticle;
	while (true)
	{
		if (stopWork)
			break;
		//ako je lista soketa pablisera prazna, odmori sekundu ------- ubaciti semafor umjesto ovoga
		if (publisherRoot == NULL)
		{
			Sleep(1000);
			continue;
		}

		//za svakog pablisera u listi provjeri da li je poslao clanak, ako jeste smjesti ga, na osnovu teme, u odgovarajuci red
		for (SocketNode* current = publisherRoot; current->next != NULL; current = current->next)
		{
			FD_ZERO(&set);
			FD_SET(current->clientSocket, &set);

			timeVal.tv_sec = 0;
			timeVal.tv_usec = 0;
			iResult = select(0, &set, NULL, NULL, &timeVal);

			//ako dodje do greske, vjerovatno je klijent nasilno zatvorio konekciju, izbaci ga iz liste
			if (iResult == SOCKET_ERROR)
			{
				printf("Select failed with error: %d", WSAGetLastError());
				deleteSocket(&publisherRoot, current->clientSocket);
			}
			else if (iResult != 0)
			{
				iResult = recv(current->clientSocket, recvbuf, DEFAULT_BUFLEN, 0);
				if (iResult > 0)
				{
					recvArticle = *(article*)recvbuf;
					enqueue(recvArticle.topic, recvArticle);
				}
			}
		}
	}
	
	SocketNode* current = publisherRoot;

	for (current; current != NULL; current = current->next)
	{
		//gasimo konekciju sa sabskrajberima
		iResult = shutdown(current->clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(current->clientSocket);
			WSACleanup();
			return 1;
		}
	}
	deleteList(&publisherRoot);
	printf("Closing thread for communication with publishers!\n");
	return 0;
}

DWORD WINAPI listenForPublishers(LPVOID lpParam)
{
	//soket za slusanje pablisera
	SOCKET listenForPublishersSocket = INVALID_SOCKET;
	//soket za komunikaciju sa klijentom
	SOCKET acceptedSocket = INVALID_SOCKET;
	//za smjestanje povratnih vrijednosti funkcija
	int iResult;
	//informaciona struktura adrese 
	addrinfo* resultingAddress = NULL;

	addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4 address
	hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
	hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

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

	iResult = bind(listenForPublishersSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		closesocket(listenForPublishersSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(resultingAddress);

	//stavljamo soket u neblokirajuci mod
	unsigned long int nonBlockingMode = 1;

	iResult = ioctlsocket(listenForPublishersSocket, FIONBIO, &nonBlockingMode);

	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", WSAGetLastError());
		return -1;
	}

	// Set listenSocket in listening mode
	iResult = listen(listenForPublishersSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenForPublishersSocket);
		WSACleanup();
		return 1;
	}

	FD_SET set;
	timeval timeVal;

	printf("Publisher listening socket initialized!\n");

	while (true)
	{
		if (stopWork)
			break;
		timeVal.tv_sec = 0;
		timeVal.tv_usec = 0;
		FD_ZERO(&set);
		FD_SET(listenForPublishersSocket, &set);
		iResult = select(0, &set, NULL, NULL, &timeVal);
		if (iResult > 0)
		{
			//otvaranje konekcije izmedju klijenta i servera
			acceptedSocket = accept(listenForPublishersSocket, NULL, NULL);

			if (acceptedSocket == INVALID_SOCKET)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(listenForPublishersSocket);
				WSACleanup();
			}
			else
			{
				printf("\tPublisher connected!\n");
				addSocket(&publisherRoot, acceptedSocket);
				//ubaciti semafor za funkciju helpPablishers
			}
		}
		else
		{
			Sleep(100);
		}
	}

	closesocket(listenForPublishersSocket);
	printf("Closing thread for publisher listening\n");
	return 0;
}

DWORD WINAPI listenForSubscribers(LPVOID lpParam)
{
	//soket za slusanje
	SOCKET listenForSubscribersSocket = INVALID_SOCKET;
	//soket za komunikaciju sa klijentom
	SOCKET acceptedSocket = INVALID_SOCKET;
	//za smjestanje povratnih vrijednosti funkcija
	int iResult;
	//buffer u koje smjestamo poruke klijenata
	char recvbuf[DEFAULT_BUFLEN];
	//informaciona struktura adrese 
	addrinfo* resultingAddress = NULL;

	addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4 address
	hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
	hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT2, &hints, &resultingAddress);
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
		freeaddrinfo(resultingAddress);
		WSACleanup();
		return 1;
	}

	iResult = bind(listenForSubscribersSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		closesocket(listenForSubscribersSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(resultingAddress);

	//stavljamo soket u neblokirajuci mod
	unsigned long int nonBlockingMode = 1;

	iResult = ioctlsocket(listenForSubscribersSocket, FIONBIO, &nonBlockingMode);

	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", WSAGetLastError());
		return -1;
	}

	//postavljamo u slusajuci rezim
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

	printf("Subscribers listening socket initialize!\n");

	while (true)
	{
		//provjera da li je mejn javio da se program gasi
		if (stopWork)
			break;
		timeVal.tv_sec = 0;
		timeVal.tv_usec = 0;
		FD_ZERO(&set);
		FD_SET(listenForSubscribersSocket, &set);
		iResult = select(0, &set, NULL, NULL, &timeVal);

		if (iResult > 0)
		{
			//Otvaranje konekcije izmedju klijenta i servera
			acceptedSocket = accept(listenForSubscribersSocket, NULL, NULL);

			if (acceptedSocket == INVALID_SOCKET)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(listenForSubscribersSocket);
				WSACleanup();
			}
			else
			{
				//nakon sto se konektuje, klijent odmah salje prvu tema na koju se sabskrajbovao(0x01,0x02,0x03,0x04,0x05)
				//i biva smjesten u jedan od rjecnika
				printf("\tSubscriber connected!\n");
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
						printf("\t\tSubscriber subscribed to topic. He is being put in dictionary with key: %c\n", recvbuf[0]);
						//smjestanje komunikacionog soketa u dictionary cija je vrijednost lista soketa
						add(recvbuf[0], acceptedSocket);
						//ubaciti semafor
						break;
					}
				}

			}
		}
		else
		{
			Sleep(100);
		}
	}

	closesocket(listenForSubscribersSocket);
	printf("Closing thread for subscriber listening\n");
	return 0;
}

DWORD WINAPI helpSubscribers(LPVOID lpParam)
{
	//0x01,0x02,0x03,0x04,0x05
	char tema = *(char*)lpParam;
	//za smjestanje povratnih vrijednosti funkcija
	int iResult;
	//buffer u koje smjestamo poruke klijenata
	char recvbuf[DEFAULT_BUFLEN];
	HANDLE semArray[2] = { sems[0], sems[tema] };
	printf("Thread with key: %x started\n", tema);
	while (true)
	{
		WaitForMultipleObjects(2, semArray, false, INFINITE);
		if (stopWork)
			break;
	}

	SocketNode* nodes = getAllSockets(tema);
	SocketNode* current = nodes;

	for (current; current != NULL; current = current->next)
	{
		//gasimo konekciju sa sabskrajberima
		iResult = shutdown(current->clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(current->clientSocket);
			WSACleanup();
			return 1;
		}
	}
	//oslobadjamo zauzetu memoriju
	deleteList(&nodes);
	//vracamo se u main tred
	printf("Closing thread for communication with subscribers\n");
	return 0;
}

void semaphoreRelease(char tema)
{
	switch (tema)
	{
		case 0x01:
			ReleaseSemaphore(sems[0], 1, NULL);
			break;
		case 0x02:
			ReleaseSemaphore(sems[1], 1, NULL);
			break;
		case 0x03:
			ReleaseSemaphore(sems[2], 1, NULL);
			break;
		case 0x04:
			ReleaseSemaphore(sems[3], 1, NULL);
			break;
		case 0x05:
			ReleaseSemaphore(sems[4], 1, NULL);
			break;
	}
}