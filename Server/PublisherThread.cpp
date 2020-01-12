#include "PublisherThread.h"
#define DEFAULT_BUFLEN3 512


extern SocketNode* publisherRoot = NULL;

DWORD __stdcall helpPublishers(LPVOID lpParam)
{
	char recvbuf[DEFAULT_BUFLEN3];
	FD_SET set;
	timeval timeVal;
	int iResult;
	struct article recvArticle;
	while (true)
	{
		if (publisherRoot == NULL)
		{
			Sleep(1000);
			continue;
		}

		for(struct SocketNode* current = publisherRoot; current->next != NULL; current = current->next)
		{
			FD_ZERO(&set);
			FD_SET(current->clientSocket, &set);

			timeVal.tv_sec = 0;
			timeVal.tv_usec = 0;
			iResult = select(0, &set, NULL, NULL, &timeVal);

			if (iResult == SOCKET_ERROR)
			{
				printf("Select failed with error: %d", WSAGetLastError());
				deleteSocket(&publisherRoot, current->clientSocket);
			}
			else if (iResult != 0)
			{
				iResult = recv(current->clientSocket, recvbuf, DEFAULT_BUFLEN3, 0);
				if (iResult > 0)
				{
					recvArticle = *(article*)recvbuf;
					enqueue(recvArticle.topic, recvArticle);
				}
			}
		}
	}

	return 0;
}
