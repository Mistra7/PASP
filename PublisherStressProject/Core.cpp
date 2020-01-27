#include "Core.h"

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

char chooseTopic() {
    char topic;
    int c;

    do {
        system("cls");
        printf("1 - GAMING\t");
        printf("2 - TECHNOLOGY\t");
        printf("3 - MEMES\t");
        printf("4 - CELEBRITIES\t");
        printf("5 - SPORT\n");

        printf("Choose a topic: ");
        topic = getchar();
        while ((c = getchar()) != '\n' && c != EOF) {}
    } while (topic < '1' || topic > '5');

    return topic;
}

void ToContinue() {
    printf("\nPress ENTER key to continue...");
    int c = getchar();
}
