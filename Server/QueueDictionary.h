#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Queue.h"

#define HASHSIZE2 100

//opis: key value par
struct nlist2 {
	char key;
	queue* articleQueue;
	struct nlist2* next;
};

//opis: hash tabela
static struct nlist2* hashTab2[HASHSIZE2];

#pragma region funkcije za rad sa rjecnikom
/*
opis: stvaranje hash vrijednosti
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: hash vrijendost
*/
unsigned hash2(char s);

/*
opis:gleda da li za zadatu vrijednost postoji key, value par u rjecniku
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: key value par
*/
struct nlist2* lookup2(char s);

/*
opis: stvaranje novog key, value para u rjecniku (vrsi provjeru da li vec postoji)
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: key value par
*/
struct nlist2* install2(char key);

/*
opis: dodaje novi clanak u odgovarajuc red (vrsi provjeri da li vec postoji key value par, ako ne, pravi novi)
parametar: karakter na osnovu kojeg se kreira hash, clanak koji ubacujemo
povratna vrijednost: nema
*/
void enqueue(char key, article value);

/*
opis: vraca prvi clan reda i oslobadja njegovu memoriju, ako je red prazan, vraca NULL
parametar: karakter na osnovu kojeg se kreira hash, soket koji izbacujemo
povratna vrijednost: nema
*/
article dequeue(char key);

/*
opis: unistava key value par u rjecniku
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: nema
*/
void destroyKeyValueQD(char key);

/*
opis: provjerava dali postoji clanak u redu
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: 0 (nema) ili 1 (ima)
*/
int checkIfQueueEmpty(char key);
#pragma endregion funkcije za rad sa rjecnikom
#pragma once
