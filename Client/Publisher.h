#pragma once
#include "Core.h"

/*
Opis: Meni za publisher-e
Param: Nema
Povratni: Odabrana opcija korisnika
*/
int publisherMenu();

/*
Opis: Omogucava pravljenje post-a
Param: Naziv autora
Povratni: Post koji sadrzi naziv autora, topic i poruku
*/
article createArticle(char * author);