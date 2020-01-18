#include "SocketList.h"

void addSocket(SocketNode** root, SOCKET s) 
{
	if (*root == NULL)
	{
		SocketNode* newNode = (SocketNode*)malloc(sizeof(SocketNode));
		if (newNode != NULL)
		{
			newNode->clientSocket = s;
			newNode->next = NULL;
			*root = newNode;
		}
	}
	else
	{
		SocketNode* current = *root;
		while (current->next != NULL)
		{
			current = current->next;
		}

		SocketNode* newNode = (SocketNode*)malloc(sizeof(SocketNode));
		if (newNode != NULL)
		{
			newNode->clientSocket = s;
			newNode->next = NULL;
			current->next = newNode;
		}
		
	}
}

void deleteSocket(SocketNode** root, SOCKET s)
{
	if (*root != NULL)
	{
		SocketNode* current = *root;
		SocketNode* last = NULL;

		if (current->clientSocket == s)
		{
			*root = (*root)->next;
			current->clientSocket = NULL;
			current->next = NULL;
			current = NULL;
			free(current);			
			return;
		}

		while (current->next != NULL)
		{
			last = current;
			current = current->next;

			if (current->clientSocket == s)
			{
				last->next = current->next;
				current->clientSocket = NULL;
				current->next = NULL;
				current = NULL;
				free(current);
				return;
			}
		}
	}
}

int checkIfExists(SocketNode** root, SOCKET s)
{
	if (*root != NULL)
	{
		SocketNode* current = *root;

		while (current->next != NULL)
		{
			if (current->clientSocket == s)
				return 1;
			else
				current = current->next;
		}
	}
	return 0;
}

SocketNode* getNext(SocketNode** root)
{
	return (*root)->next;
}

void deleteList(SocketNode** root)
{
	SocketNode* current = *root;
	SocketNode* next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}

	*root = NULL;
}