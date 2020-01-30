#pragma once
#include <winsock.h>

/*
Opis: Za proveru da li je doslo do nekog dogadjaja na soketu
Param: Soket nad kojim se ocekuje neki dogadjaj
i bool od kog zavisi rezim select-a (true za pisanje na soket, false za citanje)
Povratni: Nema
*/
void Select(SOCKET, bool);