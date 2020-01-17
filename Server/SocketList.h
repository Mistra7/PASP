#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

typedef struct SocketNode {
	SOCKET clientSocket;
	struct SocketNode* next;
}SocketNode;

#pragma region funkcije za rad sa listom
//ubacuje soket u listu, ako lista ne postoji, kreira je
void addSocket(SocketNode** root, SOCKET s);
//brise soket iz liste i prevezuje je
void deleteSocket(SocketNode** root, SOCKET s);
//provjearava da li postoji soket u listi
int checkIfExists(SocketNode** root, SOCKET s);
//vraca sledeci iz liste
SocketNode* getNext(SocketNode** root);
//brise citavu listu
void deleteList(SocketNode** root);
#pragma endregion funkcije za rad sa listom
