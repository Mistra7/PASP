#pragma once
#include <stdio.h>
#include <stdlib.h>

#pragma region strukture
typedef struct article {
	char topic; //0x01, 0x02, 0x03, 0x04, 0x05 (games, tehnology, memes, celebrities, sport, respektivno)
	char authorName[20];
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

#pragma endregion strukture

#pragma region funkcije za rad sa redom
//dodajemo clanak u red
void addToQueue(queue* q, article value);
//vadimo clanak iz reda i oslobadajmo njegovu memoriju
article* removeFromQueue(queue* q);
//inicijalizacija reda
void InitQ(queue* q);
#pragma endregion funkcije za rad sa redom