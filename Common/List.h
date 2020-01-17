#pragma once
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

/*
	Genericki cvor, sadrzi pokazivac na podatke i pokazivac na naredni cvor
*/
struct node {
	void* data;
	struct node* next;
};

/*
	Opis: dodaje novi cvor na kraj liste
	Param: prima adresu pokazivaca na prvi cvor, void pokazivac na vrednost koja se dodaje u listu 
	i velicina podataka koji se cuvaju u cvoru
	Povratna: nema
*/
void AddToList(node**, void*, size_t);

/*
	Opis: broji elemente u listi
	Param: pokazivac na prvi cvor
	Povratna : broj elemenata u listi
*/
int Count(node*);

/*
	Opis: vraca podatke koji se nalaze na odredjenom indeksu
	Param: pokazivac na prvi cvor i indeks trazenog cvor (od 0)
	Povratna: ako je ispravan indeks vraca void pokazivac na trazene podatke, u suprotnom vraca NULL
*/
void* ElementAt(node*, int);

/*
	Opis: brise poslednji cvor iz liste i oslobadja podatke u njemu
	Param: adresa pokazivaca na prvi cvor
	Povratna: nema
*/
void RemoveLast(node**);

/*
	Opis: prazni celu listu
	Param: adresa pokazivaca na prvi cvor
	Povratna: nema
*/
void ClearList(node**);