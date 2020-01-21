#pragma once
#include "Core.h"

/*
Opis: Meni sa mogucnostima subscriber-a
Param: Nema
Povratni: Izbor korisnika
*/
int SubscriberMenu();

/*
Opis: Proverava da li je korisnik pretplacen na topic
Param: Topic koji se proverava i broj topic-a na koje je korisnik pretplacen
Povratni: True ako je pretplacen, false ako nije
*/
bool isSubbed(char, int);

/*
Opis: Pretplata na odredjeni topic
Param: Poruka koja ce se slati na server i adresa duzine poruke
Povratni: Nema
*/
void SubToTopic(char*, int*);

/*
Opis: Ispis post-a
Param: Post
Povratni: Nema
*/
void printArticle(article);

/*
Opis: Ispisuje sve post-ove koje je korisnik dobio od servera i brise ih
Param: Adresa pokazivaca na listu primljenih post-ova
Povratni: Nema
*/
void DisplayPosts(node**);
