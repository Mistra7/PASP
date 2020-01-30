#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

typedef struct SocketNode {
	SOCKET clientSocket;
	struct SocketNode* next;
}SocketNode;

#pragma region funkcije za rad sa listom
/*
opis: ubacuje soket u listu, ako lista ne postoji, kreira je
parametri: pokazivac na prvi element liste, soket koji ubacujemo
//povratna vrijednost: nema
*/
void addSocket(SocketNode** root, SOCKET s);

/*
opis: brise soket iz liste, oslobadja njegovu memoriju i prevezuje je
parametri: pokazivac na prvi element liste, soket koji izbacujemo
povratna vrijednost: nema
*/
void deleteSocket(SocketNode** root, SOCKET s);

/*
opis: provjearava da li postoji soket u listi
parametri: pokazivac na prvi element liste, soket koji provjeravamo
povratna vrijednost: 0 (ne postoji) ili 1 (postoji)
*/
int checkIfExists(SocketNode** root, SOCKET s);

/*
opis: vraca sledeci iz liste
parametri: trenutni element
povratna vrijednost: sledeci node
*/
SocketNode* getNext(SocketNode** root);

/*
opis: brise citavu listu
parametri: prvi elemnt liste
povratna vrijednost: nema
*/
void deleteList(SocketNode** root);

/*
opis: broj elemenata u listu
parametri: prvi elemnt liste
povratna vrijednost: broj elemenata
*/
int listLenght(SocketNode* root);
#pragma endregion funkcije za rad sa listom
