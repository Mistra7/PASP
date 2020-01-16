#pragma once
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

/*
* genericki cvor, moze i za red da se koristi
* sadrzi pokazivac na podatke i pokazivac na naredni cvor
*/
struct node {
	void* data;
	struct node* next;
};

/*
	ukratko: dodaje novi cvor na kraj liste
	param: prima adresu pokazivaca na prvi cvor, void pokazivac na vrednost koja se dodaje u listu 
	i velicina podataka koji se cuvaju u cvoru
	povratna: nema
*/
void AddToList(node**, void*, size_t);

/*
* ukratko: broji elemente u listi
* param: pokazivac na prvi cvor
* povratna : broj elemenata u listi
*/
int Count(node*);

/*
* ukratko: vraca podatke koji se nalaze na odredjenom indeksu
* param: pokazivac na prvi cvor i indeks trazenog cvor (od 0)
* povratna: ako je ispravan indeks vraca void pokazivac na trazene podatke, u suprotnom vraca NULL
*/
void* ElementAt(node*, int);

/*
* ukratko: brise poslednji cvor iz liste i oslobadja podatke u njemu
* param: adresa pokazivaca na prvi cvor
* povratna: nema
*/
void RemoveLast(node**);

/*
* ukratko: prazni celu listu
* param: adresa pokazivaca na prvi cvor
* povratna: nema
*/
void ClearList(node**);