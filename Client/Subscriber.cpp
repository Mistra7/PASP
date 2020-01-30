#include "Core.h"

int SubscriberMenu() {
    int option, c;

    do {
        system("cls");
        printf("1. Subscribe to a topic\n");
        printf("2. Read posts\n");
        printf("3. Exit\n");

        printf("Choose an option: ");
        scanf_s("%d", &option);
        while ((c = getchar()) != '\n' && c != EOF) {}
    } while (option < 1 || option > 3);

    return option;
}

bool isSubbed(char topic, int topics_subbed) {
    bool subbed = false;

    if (topics_subbed == 5) {
        subbed = true;
    }
    else {
        for (int i = 0; i < topics_subbed; i++) {
            if (topic == *(char*)ElementAt(subbedTopics, i)) {
                subbed = true;
            }
        }
    }

    return subbed;
}

void SubToTopic(char* messageToSend, int* length) {
    char topic;
    int topics_subbed = Count(subbedTopics);
    if (topics_subbed != 5) {
        topic = chooseTopic();

        if (isSubbed(topic, topics_subbed)) {
            printf("Already subscribed to topic\n");
            ToContinue();
        }
        else {
            AddToList(&subbedTopics, &topic, sizeof(topic));
            messageToSend[(*length)++] = topic;
        }
    }
    else
    {
        system("cls");
        printf("Subscribed to all topics\n");
        ToContinue();
    }

}

void printArticle(article post) {
    switch (post.topic)
    {
    case ('1'): printf("GAMING\n");
        break;
    case ('2'): printf("TECHNOLOGY\n");
        break;
    case ('3'): printf("MEMES\n");
        break;
    case ('4'): printf("CELEBRITIES\n");
        break;
    case ('5'): printf("SPORT\n");
        break;
    default:
        break;
    }

    printf("%s\n", post.text);
    printf("by %s\n", post.authorName);
}

void DisplayPosts(node** posts) {
    int postCnt = Count(*posts);
    system("cls");

    if (postCnt == 0) {
        printf("No new posts have been received\n");
    }
    else {
        for (int i = 0; i < postCnt; i++)
        {
            printArticle(*(article*)ElementAt(*posts, i));
            printf("---------------------------------------------\n");
        }
        ClearList(posts);
    }
    ToContinue();
}