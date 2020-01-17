#include "List.h"

void AddToList(node** head, void* data, size_t size) {
	node* newNode = (node*)malloc(sizeof(node));
	if (newNode != NULL) {
		newNode->data = malloc(size);
		memcpy(newNode->data, data, size);
		newNode->next = NULL;
	}
	else
	{
		return;
	}
	
	if (*head == NULL) {
		*head = newNode;
	}
	else {
		node* temp = *head;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = newNode;
	}
}

int Count(node* head) {
	int cnt = 0;
	node* temp = head;
	
	while (true) {
		if (temp == NULL) {
			break;
		}
		else
		{
			++cnt;
			temp = temp->next;
		}
	}

	return cnt;
}

void* ElementAt(node* head, int index) {
	void* retValue;

	if (head == NULL) {
		printf("List is empty\n");
		retValue = NULL;
	}
	else if (index < 0 || index >= Count(head)) {
		printf("Index is out of bounds\n");
		retValue = NULL;
	}
	else {
		int cnt = 0;
		node* temp = head;
		while (cnt != index) {
			++cnt;
			temp = temp->next;
		}
		retValue = temp->data;
	}

	return retValue;
}

void RemoveLast(node** head) {
	if (head == NULL) {
		printf("List is empty\n");
		return;
	}
	else if ((*head)->next == NULL) {
		free((*head)->data);
		free(*head);
		*head = NULL;
	}
	else {
		node* temp = *head;
		while (temp->next->next != NULL) {
			temp = temp->next;
		}
		free(temp->next->data);
		free(temp->next);
		temp->next = NULL;
	}
}

void ClearList(node** head) {
	while (*head != NULL) {
		RemoveLast(head);
	}
}