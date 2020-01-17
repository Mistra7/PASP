#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct article {
	char topic; //0x01, 0x02, 0x03, 0x04, 0x05
	char authorName[20]; //'\0'
	char text[101];
}article;

struct queueNode {
	struct article value;
	struct queueNode* next;
};

struct queue {
	struct queueNode* first;
	struct queueNode* last;
};

//dodajemo clanak u red
void addToQueue(queue* q, article value);
//vadimo clanak iz reda i oslobadajmo njegovu memoriju
article* removeFromQueue(queue* q);
//inicijalizacija reda
void InitQ(queue* q);