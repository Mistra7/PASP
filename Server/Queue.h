#pragma once
struct queueNode {
	int value;
	queueNode* next;
};

struct queue {
	queueNode* first;
	queueNode* last;
};

void Add(queue* q, int value) {
	queueNode* newNode = (queueNode*)malloc(sizeof(queueNode));
	newNode->value = value;
	newNode->next = NULL;

	if (q->first == NULL && q->last == NULL) {
		q->first = newNode;
	}
	else {
		q->last->next = newNode;
	}
	q->last = newNode;
}

int Remove(queue* q) {
	if (q->first == NULL) {
		printf("Queue is empty\n");
		return -1;
	}

	int ret;
	queueNode* temp = q->first;

	if (q->first->next == NULL) {
		q->last = NULL;
	}

	ret = q->first->value;
	q->first = temp->next;
	free(temp);

	return ret;
}

void InitQ(queue* q) {
	q->first = NULL;
	q->last = NULL;
}