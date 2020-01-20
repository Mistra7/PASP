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

void Select(SOCKET socket, bool write) {
    // Initialize select parameters
    FD_SET set;
    timeval timeVal;

    // Set timeouts to zero since we want select to return
    // instantaneously
    timeVal.tv_sec = 0;
    timeVal.tv_usec = 0;

    int iResult;

    do {
        FD_ZERO(&set);
        // Add socket we will wait to read from
        FD_SET(socket, &set);

        if (write) {
            iResult = select(0, NULL, &set, NULL, &timeVal);
        }
        else {
            iResult = select(0, &set, NULL, NULL, &timeVal);
        }

        // lets check if there was an error during select
        if (iResult == SOCKET_ERROR)
        {
            fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
            continue;
        }

        // now, lets check if there are any sockets ready
        if (iResult == 0)
        {
            // there are no ready sockets, sleep for a while and check again
            Sleep(50);
            continue;
        }
    } while (iResult != 1);
}