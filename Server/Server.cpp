#pragma comment(lib, "Ws2_32.lib")
#include "Threads.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "20000"
#define DEFAULT_PORT2 "20001"
#define SERVER_SLEEP_TIME 50

bool InitializeWindowsSockets();
int stopWork = 0;
CRITICAL_SECTION stopWorkCS, lDictionary[5], qDictionary[5];
HANDLE sems[6];

int main()
{
	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}

	#pragma region inicijalizacija semafora i kriticnih sekcija
	InitializeCriticalSection(&stopWorkCS);
	sems[0] = CreateSemaphore(0, 0, 5, NULL);
	
	for (int i = 0; i < 5; i++)
	{
		sems[i + 1] = CreateSemaphore(0, 0, 1, NULL);
		InitializeCriticalSection(&lDictionary[i]);
		InitializeCriticalSection(&qDictionary[i]);	
	}
#pragma endregion inicijalizacija semafora i kriticnih sekcija

	#pragma region inicijalizacija tredova
	DWORD listenSubscribresID; //listenPublishersID
	DWORD helpPublishersID;
	//0x01, 0x02, 0x03, 0x04, 0x05 --respektivno
	DWORD subGamesID, subTehnologyID, subMemesID, subCelebsID, subSportID;
	HANDLE listenSubscribres; //listenPublishers
	HANDLE helpPub;
	HANDLE subGames, subTehnology, subMemes, subCelebs, subSport;

	char tema1 = 0x01;
	char tema2 = 0x02;
	char tema3 = 0x03;
	char tema4 = 0x04;
	char tema5 = 0x05;

	//listenPublishers = CreateThread(NULL, 0, &listenForPublishers, NULL, 0, &listenPublishersID);
	subGames = CreateThread(NULL, 0, &helpSubscribers, &tema1, 0, &subGamesID);
	subTehnology = CreateThread(NULL, 0, &helpSubscribers, &tema2, 0, &subTehnologyID);
	subMemes = CreateThread(NULL, 0, &helpSubscribers, &tema3, 0, &subMemesID);
	subCelebs = CreateThread(NULL, 0, &helpSubscribers, &tema4, 0, &subCelebsID);
	subSport = CreateThread(NULL, 0, &helpSubscribers, &tema5, 0, &subSportID);
	HANDLE waitForThreads[5] = { subGames, subTehnology, subMemes, subCelebs, subSport };
	listenSubscribres = CreateThread(NULL, 0, &listenForSubscribers, &waitForThreads, 0, &listenSubscribresID);
	helpPub = CreateThread(NULL, 0, &helpPublishers, &waitForThreads, 0, &helpPublishersID);
	#pragma endregion inicijalizacija tredova

	#pragma region zatvaranje programa i klinap
	HANDLE array2[7] = { listenSubscribres, helpPub, subGames, subTehnology, subMemes, subCelebs, subSport };
	Sleep(100);
	printf("\n\t\tPress any key to leave\n");
	char lit = getchar();
	EnterCriticalSection(&stopWorkCS);
	stopWork = 1;
	LeaveCriticalSection(&stopWorkCS);
	ReleaseSemaphore(sems[0], 5, NULL);
	WaitForMultipleObjects(7, array2, true, INFINITE);

	for (int i = 0; i < 7; i++)
	{
		CloseHandle(array2[i]);
		if(i < 6)
			CloseHandle(sems[i]);
		if (i < 5)
		{
			DeleteCriticalSection(&lDictionary[i]);
			DeleteCriticalSection(&qDictionary[i]);
		}
	}
	DeleteCriticalSection(&stopWorkCS);

	WSACleanup();
	#pragma endregion zatvaranje programa i klinap


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