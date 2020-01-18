#include "Threads.h"

//adrese pablisera
SocketNode* publisherRoot = NULL;
#define DEFAULT_BUFLEN 512
//port za komunikaciju sa pabliserima
#define DEFAULT_PORT "20000"
//port za komunikaciju sa klijentima
#define DEFAULT_PORT2 "20001"

#pragma region Tred funkcije

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
	printf("help Publishers started!\n");

	//opsluzivanje pablisera
	while (true)
	{
		if (criticalStopWork())
		{
			break;
		}
			
		//ako je lista soketa pablisera prazna, odmori sekundu
		EnterCriticalSection(&pubList);
		SocketNode* current = publisherRoot;
		LeaveCriticalSection(&pubList);
		if (current == NULL)
		{
			Sleep(1000);
			continue;
		}

		//za svakog pablisera u listi provjeri da li je poslao clanak, ako jeste smjesti ga, na osnovu teme, u odgovarajuci red
		for (current ; current->next != NULL; current = current->next)
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
					int n = atoi(&recvArticle.topic);
					EnterCriticalSection(&qDictionary[n - 1]);
					enqueue(recvArticle.topic, recvArticle);
					LeaveCriticalSection(&qDictionary[n - 1]);
					ReleaseSemaphore(sems[n], 1, NULL);
				}
			}
		}
	}
	
	#pragma region ciscenje memorije i zatvaranje treda
	SocketNode* current = publisherRoot;

	for (current; current != NULL; current = current->next)
	{
		//gasimo konekciju sa pabliserima
		iResult = shutdown(current->clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(current->clientSocket);
			WSACleanup();
		}
	}
	deleteList(&publisherRoot);
	printf("Closing thread for communication with publishers!\n");
	#pragma endregion ciscenje memorije i zatvaranje treda

	return 0;
}

DWORD WINAPI listenForPublishers(LPVOID lpParam)
{
	#pragma region Otvaranje novog listen soketa
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
#pragma endregion Otvaranje novog listen soketa

	//Novi klijenti
	while (true)
	{
		if (criticalStopWork())
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
				EnterCriticalSection(&pubList);
				addSocket(&publisherRoot, acceptedSocket);
				LeaveCriticalSection(&pubList);
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
	#pragma region Pokretanje neblokirajuceg listen soketa
	SocketNode* subList = NULL;
	//soket za slusanje
	SOCKET listenForSubscribersSocket = INVALID_SOCKET;
	//soket za komunikaciju sa klijentom
	SOCKET acceptedSocket = INVALID_SOCKET;
	//za smjestanje povratnih vrijednosti funkcija
	int iResult;
	//buffer u koje smjestamo poruke klijenata
	//char recvbuf[DEFAULT_BUFLEN];
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

	#pragma endregion Pokretanje neblokirajuceg listen soketa

	while (true)
	{
		//provjera da li je mejn javio da se program gasi
		if (criticalStopWork())
			break;

		#pragma region Novi klijenti
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
				//closesocket(listenForSubscribersSocket);
				WSACleanup();
			}
			else if(iResult > 0)
			{
				printf("New subscriber connected\n");
				addSocket(&subList, acceptedSocket);
			}
			
		}
	#pragma endregion Novi klijenti

		#pragma region Nove sabskripcije
		if (subList != NULL)
		{
			SocketNode* current = subList;
			for (current; current != NULL; current = current->next)
			{
				FD_SET set;
				timeval timeVal;

				timeVal.tv_sec = 0;
				timeVal.tv_usec = 0;
				FD_ZERO(&set);
				// Add socket we will wait to read from
				FD_SET(current->clientSocket, &set);

				iResult = select(0, &set, NULL, NULL, &timeVal);

				if (iResult == SOCKET_ERROR)
				{
					fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
					deleteSocket(&subList, current->clientSocket);
				}

				if (iResult != 0)
				{
					char re[1];
					iResult = recv(current->clientSocket, re, 1, 0);
					if (iResult == SOCKET_ERROR)
					{
						fprintf(stderr, "recive failed with error: %ld\n", WSAGetLastError());
						deleteSocket(&subList, current->clientSocket);
					}
					else if (iResult == 0)
					{
						printf("Client has closed the connection! Deleting him from list!\n");
						closesocket(current->clientSocket);
						deleteSocket(&subList, current->clientSocket);
					}
					else
					{

						int n = atoi(re);
						EnterCriticalSection(&lDictionary[n - 1]);
						add(n, current->clientSocket);
						LeaveCriticalSection(&lDictionary[n - 1]);
						printf("New subscription on topic %s\n", giveMeTopic(n));
					}
				}
			}
		}		
		Sleep(50);
	#pragma endregion Nove sabskripcije
	}


	#pragma region ciscenje memorije i zatvaranje treda

	SocketNode* current = subList;
	//gasenje klijenata
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


	deleteList(&subList);

	closesocket(listenForSubscribersSocket);
	printf("Closing thread for subscriber listening\n");
	#pragma endregion ciscenje memorije i zatvaranje treda

	return 0;
}

DWORD WINAPI helpSubscribers(LPVOID lpParam)
{
	//parametar koji nam govori za koje je klijente/teme tred odgovoran (0x01, 0x02, 0x03, 0x04, 0x05)
	char tema = *(char*)lpParam;
	//za smjestanje povratnih vrijednosti funkcija
	int iResult;
	//semafori na koje cekamo
	HANDLE semArray[2] = { sems[0], sems[tema] };
	printf("Thread with key: %x started\n", tema);
	while (true)
	{
		//cekamo semafor koji nam javlja da ima clanaka za poslati ili semafor koji nam govori da 
		//tred treba da se ugasi
		WaitForMultipleObjects(2, semArray, false, INFINITE);
		if (criticalStopWork())
			break;
		
		EnterCriticalSection(&lDictionary[tema - 1]);
		SocketNode* head = getAllSockets(tema);
		LeaveCriticalSection(&lDictionary[tema - 1]);

		//ako nema kome da se salje, clanci ostaju u redu
		if (head == NULL)
		{
			continue;
		}

		//iteracija kroz clanke koje saljemo
		for (article* art = criticalDequeue(tema); art != NULL; art = criticalDequeue(tema))
		{
			printf("\tSending article with authors name: %s to clients\n", art->authorName);

			//iteracija kroz sokete i slanje clanaka
			for (head; head != NULL; head = criticalNext(head, tema))
			{
				// Initialize select parameters
				FD_SET set;
				timeval timeVal;

				//provjeravamo da li mozemo klijentu da posaljemo clanak
				do {
					// Set timeouts to zero since we want select to return
					// instantaneously
					timeVal.tv_sec = 0;
					timeVal.tv_usec = 0;
					FD_ZERO(&set);
					// Add socket we will wait to read from
					FD_SET(head->clientSocket, &set);

					iResult = select(0, NULL, &set, NULL, &timeVal);

					// lets check if there was an error during select
					if (iResult == SOCKET_ERROR)
					{
						fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
						EnterCriticalSection(&lDictionary[tema - 1]);
						removeValue(tema, head->clientSocket);
						LeaveCriticalSection(&lDictionary[tema - 1]);
						break;
					}

					// now, lets check if there are any sockets ready
					if (iResult == 0)
					{
						// there are no ready sockets, sleep for a while and check again
						Sleep(10);
						continue;
					}
				} while (iResult < 1);
				
				//slanje clanka
				if (iResult >= 1)
				{
					iResult = send(head->clientSocket, (const char*)art, sizeof(*art), 0);
					if (iResult == SOCKET_ERROR) 
					{
						fprintf(stderr, "send failed with error: %ld\n", WSAGetLastError());
						//ako je doslo do greske, pretpostavljamo da klijent prekinuo konekciju te izbacujemo njegov soket
						EnterCriticalSection(&lDictionary[tema - 1]);
						removeValue(tema, head->clientSocket);
						LeaveCriticalSection(&lDictionary[tema - 1]);
					}
				}
			}
		}
		
	}

	#pragma region ciscenje memorije i zatvaranje treda
	SocketNode* nodes = getAllSockets(tema);
	//ako imamo preostalih clanaka za slanje, oslobodimo memoriju
	while (dequeue(tema) != NULL)
	{}
	//oslobadjamo zauzetu memoriju za sokete
	deleteList(&nodes);
	//vracamo se u main tred
	printf("Closing thread for communication with subscribers\n");
	#pragma endregion ciscenje memorije i zatvaranje treda

	return 0;
}

#pragma endregion Tred funkcije

#pragma region Pomocne funkcije

article* criticalDequeue(char tema)
{
	EnterCriticalSection(&qDictionary[tema + 1]);
	article* ret = dequeue(tema);
	LeaveCriticalSection(&qDictionary[tema + 1]);

	return ret;
}

SocketNode* criticalNext(SocketNode* head, char tema)
{
	EnterCriticalSection(&lDictionary[tema + 1]);
	SocketNode* soc = head->next;
	LeaveCriticalSection(&lDictionary[tema + 1]);

	return soc;
}

int criticalStopWork()
{
	EnterCriticalSection(&stopWorkCS);
	int i = stopWork;
	LeaveCriticalSection(&stopWorkCS);
	return i;
}

const char* giveMeTopic(int c)
{
	switch (c)
	{
		case 1:
			return "gaming";
		case 2:
			return "technology";
		case 3:
			return "memes";
		case 4:
			return "celebrities";
		case 5:
			return "sport";
	}

	return "";
}

#pragma endregion Pomocne funkcije
