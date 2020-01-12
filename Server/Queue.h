#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"

struct queueNode {
	struct article value;
	struct queueNode* next;
};

struct queue {
	struct queueNode* first;
	struct queueNode* last;
};

void add(queue* q, article value);
article* remove(queue* q);
void InitQ(queue* q);