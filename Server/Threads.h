#pragma once
#include <ws2tcpip.h>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "QueueDictionary.h"
#include "Dictionary.h"


#pragma region Externe promjenljive
/*
opis: zajednicka promjenljiva za mejn tred i helpSubscribers tredove
oznacava kraj rada programa
vrijednosti: 0 (nastavi sa radom) ili 1(zaustavi rad)
*/
extern int stopWork;

/*
opis: semafori
sems[0] javlja kada je kraj rada (koristi se samo za helpSubscribers tredove)
sems[1-5] javlju da je ubacen uclanak u red
sems[6-10] javljaju da je stigao novi sabskrajber
*/
extern HANDLE sems[6];

/*
opis: kriticne sekcije
stopWorkCS za mjenjanje i citanje zajednicke promjenljive stopWork
lDictionary za rad sa rjecnicima u kojema se nalaze liste klijentskih(sabskrajberi) soketa
qDictionary za rad sa rjecnicima u kojema se nalaze redovi sa clancima
pubList za rad sa listom klijentskih(pabliseri) soketa
*/
extern CRITICAL_SECTION stopWorkCS, lDictionary[5], qDictionary[5], pubList;
#pragma endregion Externe promjenljive

#pragma region Pomocne funkcije
/*
opis: funkcija zaduzena za sigurno vadjenje iz reda
parametar: kljuc u rjecniku
povratna vrijednost: clanak
*/
article criticalDequeue(char tema);

/*
opis: funkcija zaduzena za sigurno preuzimanje sledeceg elemnta iz liste
parametri: trenutni element liste, kljuc u rjecniku
povratna vrijednost: sledeci elemnt u listu ili null ako je kraj liste
*/
SocketNode* criticalNext(SocketNode* head, char tema);

/*
opis: funkcija za sigurno preuzimanje promjenjive stopWork
parametri: nema
povratna vrijednost: 0 (nastavi sa radom) ili 1(kraj rada)
*/
int criticalStopWork();

/*
opis: vraca naziv teme na osnovu njene brojne protivvrijednosti
parametri: broj od 1 do 5
povratna vrijednost: naziv teme
*/
const char* giveMeTopic(int c);

/*
opis: da li se u listi(koja je value u rjecniku) nalazi baram jedan soket
parametri: karakter(tema) na osnovu koje vrsimo provjeru u rjecniku
povratna vrijednost: 0 (nema) ili 1 (ima)
*/
int criticalCheckList(char c);

//
/*
opis: provjera da li postoji clanaka u redu
parametri: karakter(tema) na osnovu koje vrsimo provjeru u rjecniku
povratna vrijednost: 0 (nema) ili 1 (ima)
*/
int criticalCheckQueue(char c);
#pragma endregion Pomocne funkcije

#pragma region Tred funkcije
/*
opis: funkcija zaduzena za "slusanje" sabskrajbera, kreiranje soketa, primanje tema i smjestanje soketa u odgovarajucu 
listu soketa na osnovu teme
parametar: proslijedjuje se void
povratna vrijednost: 0 ako se tred uspjesno zatvori
*/
DWORD WINAPI listenForSubscribers(LPVOID lpParam);

/*
opis: funkcija zaduzena za isporucivanje clanaka klijentima koji su sabskrajbovani na tema za koju je tred zaduzen i za
sabskrajbovanje klijenata na druge teme
parametar: tema za koju je tred zaduzen
povratna vrijednost: 0 ako se tred uspjesno zatvori
*/
DWORD WINAPI helpSubscribers(LPVOID lpParam);

/*
opis: funkcija zaduzena za "slusanje" pablisera, kreiranje soketa i smjestanje soketa u u listu pablisera
i za prijem clanaka od pablisera koje onda smjesta u odgovarajuci red u QueueDictionary
parametar: proslijedjuje se void
povratna vrijednost: 0 ako se tred uspjesno zatvori
*/
DWORD WINAPI helpPublishers(LPVOID lpParam);
#pragma endregion Tred funkcije


