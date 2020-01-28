#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Queue.h"

#define HASHSIZE2 100

struct nlist2 {
	char key;
	queue* articleQueue;
	struct nlist2* next;
};

static struct nlist2* hashTab2[HASHSIZE2];

#pragma region funkcije za rad sa rjecnikom
//stvaranje hash vrijednosti
unsigned hash2(char s);
//gleda da li za zadatu vrijednost postoji  key, value par u rjecniku
struct nlist2* lookup2(char s);
//stvaranje novog key, value para u rjecniku (vrsi provjeru da li vec postoji)
struct nlist2* install2(char key);
//ubacuje prvi clanak u red. Ako red ne postoji, stvara ga pozivanjem install2
void enqueue(char key, article value);
//vraca prvi clan reda i oslobadja njegovu memoriju, ako je red prazan, vraca NULL
article dequeue(char key);
//unistava key value par u rjecniku
void destroyKeyValueQD(char key);
//provjerava da li postoji clanak u redu
int checkIfQueueEmpty(char key);
#pragma endregion funkcije za rad sa rjecnikom
#pragma once
