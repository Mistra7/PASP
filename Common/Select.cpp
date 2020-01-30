#include "Select.h"
#include <stdio.h>

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