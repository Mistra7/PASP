#include "ReceiveThread.h"

DWORD WINAPI receive(LPVOID lpParam) {
    threadParam tp = *(threadParam*)lpParam;
    int iResult;
    char recvbuf[DEFAULT_BUFLEN] = "";

    while (1) {
        Select(tp.socket, false);

        // Receive data until the client shuts down the connection
        iResult = recv(tp.socket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0)
        {
            if (tp.type == SUBSCRIBER) {
                article* post = (article*)recvbuf;

                EnterCriticalSection(&list_cs);
                AddToList(&posts, post, sizeof(article));
                LeaveCriticalSection(&list_cs);
                printf("\nNew post received\n");
            }      
        }
        else if (iResult == 0)
        {
            // connection was closed gracefully
            printf("\nServer has been shutdown.\n");
            *tp.exit = true;
            break;
        }
        else
        {
            // there was an error during recv
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(tp.socket);
            break;
        }
    }

    return 0;
}