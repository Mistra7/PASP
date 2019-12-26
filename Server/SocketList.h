#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

struct SocketNode {
	SOCKET clientSocket;
	struct SocketNode* next;
};

void addSocket(SocketNode** root, SOCKET s);
void deleteSocket(SocketNode** root, SOCKET s);
int checkIfExists(SocketNode** root, SOCKET s);