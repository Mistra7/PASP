#pragma once
#include <conio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Common/List.h"
#include "../Common/Structs.h"

//Duzina bafera koji se salje preko mreze
#define DEFAULT_BUFLEN 123

/*
Za oznacavanje tipa korisnika
*/
enum user_type { PUBLISHER, SUBSCRIBER };

/*
Opis: Za proveru da li je doslo do nekog dogadjaja na soketu
Param: Soket nad kojim se ocekuje neki dogadjaj 
i bool od kog zavisi rezim select-a (true za pisanje na soket, false za citanje)
Povratni: Nema
*/
void Select(SOCKET, bool);

/*
Opis: Omogucava izbor topic-a
Param: Nema
Povratni: Izabran topic
*/
char chooseTopic();

/*
Opis: Press any key to continue
Param: Nema
Povratni: Nema
*/
void ToContinue();

//Lista postova koji su stigli preko mreze
extern node* posts;
//Lista topic-a na koje je korinsik pretplacen
extern node* subbedTopics;