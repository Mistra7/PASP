#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Common/Structs.h"

#pragma region strukture

struct queueNode {
	struct article value;
	struct queueNode* next;
};

struct queue {
	struct queueNode* first;
	struct queueNode* last;
};

#pragma endregion strukture

#pragma region funkcije za rad sa redom
/*
opis: dodajemo clanak u red
parametri: pokazivac na red u koji dodajemo, clanak koji ubacujemo
povratna vrijednost: nema
*/
void addToQueue(queue* q, article value);


/*
opis: vadi prvi clanak iz reda i oslobadja njegovu memoriju
paremtar: pokazivac na red iz kojeg vadimo
povratna vrijednost: clanak
*/
article removeFromQueue(queue* q);

//inicijalizacija reda
/*
opis: inicijalizacija reda
parametar: pokazivac na red
povratna vrijednost: nema
*/
void InitQ(queue* q);
#pragma endregion funkcije za rad sa redom