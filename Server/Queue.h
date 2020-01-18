#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Common/Structs.h"

#pragma region strukture

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
article removeFromQueue(queue* q);
//inicijalizacija reda
void InitQ(queue* q);
#pragma endregion funkcije za rad sa redom