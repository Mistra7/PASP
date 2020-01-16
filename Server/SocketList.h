#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

typedef struct SocketNode {
	SOCKET clientSocket;
	struct SocketNode* next;
}SocketNode;

void addSocket(SocketNode** root, SOCKET s);
void deleteSocket(SocketNode** root, SOCKET s);
int checkIfExists(SocketNode** root, SOCKET s);
SocketNode* getNext(SocketNode** root);
void deleteList(SocketNode** root);