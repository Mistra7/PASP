#include "Publisher.h"

int publisherMenu() {
	int option, c;

	do {
		system("cls");
		printf("1. Publish\n");
		printf("2. Exit\n");

		printf("Choose an option: ");
		scanf_s("%d", &option);
        while ((c = getchar()) != '\n' && c != EOF) {}
	} while (option < 1 || option > 2);

	return option;
}

article createArticle(char* author) {
    article a;
    int c;

    strcpy_s(a.authorName, author);
    a.topic = chooseTopic();
    system("cls");

    printf("Enter message: ");
    scanf_s("%[^\n]", a.text, TEXT_LEN);
    while ((c = getchar()) != '\n' && c != EOF) {}

    return a;
}
