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

	printf("Publisher listening socket initialized!\n");
#pragma endregion Otvaranje novog neblokirajuceg listen soketa

	HANDLE *waitThreads = (HANDLE*)lpParam;
	//buffer u koje smjestamo poruke klijenata
	char recvbuf[sizeof(article)];
	FD_SET set;
	timeval timeVal;
	//za smjestanje povratnih vrijednosti funkcija
	//clanak koji dobijamo od klijenta
	struct article recvArticle;
	char tp[2];
	tp[1] = '\0';
	printf("help Publishers started!\n");
	//opsluzivanje pablisera
	while (true)
	{
		if (criticalStopWork())
		{
			break;
		}

#pragma region Nova konekcija
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
			}
		}
#pragma endregion Nova konekcija
			
#pragma region Novi clanak
		//ako je lista soketa pablisera prazna, odmori sekundu
		SocketNode* current = publisherRoot;;
		if (current == NULL)
		{
			Sleep(20);
			continue;
		}
		
		//za svakog pablisera u listi provjeri da li je poslao clanak, ako jeste smjesti ga, na osnovu teme, u odgovarajuci red
		//for (current ; current != NULL; current = current->next)
		while(current != NULL)
		{
			SocketNode* temp = NULL;
			int a = 0;
			iResult = 0;
			do {
				FD_ZERO(&set);
				FD_SET(current->clientSocket, &set);

				timeVal.tv_sec = 0;
				timeVal.tv_usec = 0;
				a = select(0, &set, NULL, NULL, &timeVal);
				if (a)
				{
					a = recv(current->clientSocket, recvbuf + iResult, sizeof(article) - iResult, 0);
					if (a < 0)
					{
						//Desila se neka greska
						printf("Recv failed with error:%d\n", WSAGetLastError());
						temp = current;
						break;
					}
					else if (a == 0)
					{
						//Klijent zatvorio konekciju
						temp = current;
						break;
					}
					iResult += a;
				}
				else if (a == SOCKET_ERROR)
				{
					printf("Select failed with error: %d\n", WSAGetLastError());
					temp = current;
					break;
				}
				else if (a == 0)
				{
					//Nema sta da se primi
					break;
				}
					
			} while (iResult != sizeof(article));

			if (iResult == sizeof(article))
			{
				recvArticle = *(article*)recvbuf;
				tp[0] = recvArticle.topic;
				int n = atoi(tp);
				printf("Received article with topic %s, from author %s\n", giveMeTopic(n), recvArticle.authorName);
				EnterCriticalSection(&qDictionary[n - 1]);
				enqueue(n, recvArticle);
				LeaveCriticalSection(&qDictionary[n - 1]);
				if (criticalCheckList(n))
					ReleaseSemaphore(sems[n], 1, NULL);
			}

			current = current->next;
			if (temp != NULL)
			{
				//Ili je klijent zatvorio konekciju ili je recv propao ili je select propao
				deleteSocket(&publisherRoot, temp->clientSocket);
			}
			
		}
#pragma endregion Novi clanak
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
		}
		closesocket(current->clientSocket);
	}
	deleteList(&publisherRoot);
	closesocket(listenForPublishersSocket);

	WaitForMultipleObjects(5, waitThreads, true, INFINITE);

	for (int i = 0; i < 5; i++)
	{
		EnterCriticalSection(&qDictionary[i]);
		destroyKeyValueQD((char)(i + 1));
		LeaveCriticalSection(&qDictionary[i]);
	}
	printf("Closing thread for communication with publishers!\n");
	#pragma endregion ciscenje memorije i zatvaranje treda

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
	HANDLE* waitThreads = (HANDLE*)lpParam;
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

						if(criticalCheckQueue(n))
							ReleaseSemaphore(sems[n], 1, NULL);
					}
				}
			}
		}		
	#pragma endregion Nove sabskripcije
	}


#pragma region ciscenje memorije i zatvaranje treda
	WaitForMultipleObjects(5, waitThreads, true, INFINITE);
	SocketNode* current = subList;
	
	//gasenje klijenata
	for (current; current != NULL; current = current->next)
	{
		//gasimo konekciju sa sabskrajberima
		iResult = shutdown(current->clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
		}
		closesocket(current->clientSocket);
	}

	for (int i = 0; i < 5; i++)
	{
		EnterCriticalSection(&lDictionary[i]);
		destroyKeyValueLD((char)(i + 1));
		LeaveCriticalSection(&lDictionary[i]);
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
	HANDLE semArray[2] = { sems[0], sems[tema]};
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
		int breakSending = 0;
		//iteracija kroz clanke koje saljemo
		for (article art = criticalDequeue(tema); art.topic != '0'; art = criticalDequeue(tema))
		{
			/*printf("\tSending article with authors name: %s to clients\n", art.authorName);
			printf("\t\tArticle: %s\n", art.text);*/

			//iteracija kroz sokete i slanje clanaka
			//for (SocketNode* current = head; current != NULL; current = criticalNext(current, tema))
			SocketNode* current = head;
			SocketNode* temp = NULL;
			while(current != NULL)
			{
				// Initialize select parameters
				FD_SET set;
				timeval timeVal;

				//provjeravamo da li mozemo klijentu da posaljemo clanak
				// Set timeouts to zero since we want select to return
				// instantaneously
				timeVal.tv_sec = 0;
				timeVal.tv_usec = 0;
				FD_ZERO(&set);
				// Add socket we will wait to read from
				FD_SET(current->clientSocket, &set);

				iResult = select(0, NULL, &set, NULL, &timeVal);

				// lets check if there was an error during select
				if (iResult == SOCKET_ERROR)
				{
					if (listLenght(current) == 1)
					{
						EnterCriticalSection(&qDictionary[tema - 1]);
						enqueue(tema, art);
						LeaveCriticalSection(&qDictionary[tema - 1]);
						breakSending = 1;
					}
					printf("Client canceled connection\n");
					temp = current;
					current = current->next;
					EnterCriticalSection(&lDictionary[tema - 1]);
					removeValue(tema, temp->clientSocket);
					LeaveCriticalSection(&lDictionary[tema - 1]);
					continue;
				}
				
				//slanje clanka
				if (iResult >= 1)
				{
					iResult = send(current->clientSocket, (const char*)&art, sizeof(article), 0);
					if (iResult == SOCKET_ERROR) 
					{
						if (listLenght(current) == 1)
						{
							EnterCriticalSection(&qDictionary[tema - 1]);
							enqueue(tema, art);
							LeaveCriticalSection(&qDictionary[tema - 1]);
							breakSending = 1;
						}
						int error = WSAGetLastError();
						fprintf(stderr, "send to subscribers failed with error: %ld\n", error);
						if (error == 10035)
							continue;
						//ako je doslo do greske, pretpostavljamo da klijent prekinuo konekciju te izbacujemo njegov soket
						temp = current;
						current = current->next;
						EnterCriticalSection(&lDictionary[tema - 1]);
						removeValue(tema, temp->clientSocket);
						LeaveCriticalSection(&lDictionary[tema - 1]);
					}
					else if (iResult == 0)
					{
						temp = current;
						current = current->next;
						EnterCriticalSection(&lDictionary[tema - 1]);
						removeValue(tema, temp->clientSocket);
						LeaveCriticalSection(&lDictionary[tema - 1]);
					}
					else
					{
						current = current->next;
					}
				}
			}
			if (breakSending)
				break;
		}
		
	}

	#pragma region ciscenje memorije i zatvaranje treda

	//oslobodimo vrijednost u rjecnicima
	/*EnterCriticalSection(&qDictionary[tema - 1]);
	destroyKeyValueQD(tema);
	LeaveCriticalSection(&qDictionary[tema - 1]);
	EnterCriticalSection(&lDictionary[tema - 1]);
	destroyKeyValueLD(tema);
	LeaveCriticalSection(&lDictionary[tema - 1]);*/

	//vracamo se u main tred
	printf("Closing thread for communication with subscribers. Thread: %x\n", tema);
	#pragma endregion ciscenje memorije i zatvaranje treda

	return 0;
}

#pragma endregion Tred funkcije

#pragma region Pomocne funkcije

article criticalDequeue(char tema)
{
	EnterCriticalSection(&qDictionary[tema - 1]);
	article ret = dequeue(tema);
	LeaveCriticalSection(&qDictionary[tema - 1]);

	return ret;
}

SocketNode* criticalNext(SocketNode* head, char tema)
{
	EnterCriticalSection(&lDictionary[tema - 1]);
	SocketNode* soc = head->next;
	LeaveCriticalSection(&lDictionary[tema - 1]);

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

int criticalCheckList(char c)
{
	int a = 0;
	EnterCriticalSection(&lDictionary[c - 1]);
	if (getAllSockets(c) != NULL)
		a = 1;
	LeaveCriticalSection(&lDictionary[c - 1]);

	return a;
}

int criticalCheckQueue(char c)
{
	int a = 0;
	EnterCriticalSection(&qDictionary[c - 1]);
	a = checkIfQueueEmpty(c);
	LeaveCriticalSection(&qDictionary[c - 1]);

	return a;
}

#pragma endregion Pomocne funkcije
