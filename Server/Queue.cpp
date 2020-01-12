#include "Queue.h"

void addToQueue(queue* q, article value) {
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

article* removeFromQueue(queue* q) {
	if (q->first == NULL) {
		printf("Queue is empty\n");
		return NULL;
	}

	article* ret = NULL;
	queueNode* temp = q->first;

	if (q->first->next == NULL) {
		q->last = NULL;
	}

	*ret = q->first->value;
	q->first = temp->next;
	free(temp);

	return ret;
}

void InitQ(queue* q) {
	q->first = NULL;
	q->last = NULL;
}