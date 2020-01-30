#include "Core.h"

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
