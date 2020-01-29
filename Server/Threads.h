#pragma once
#include <ws2tcpip.h>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "QueueDictionary.h"
#include "Dictionary.h"


#pragma region Externe promjenljive
//zajednicka promjenljiva za mejn tred i helpSubscribers tredove
//oznacava kraj rada programa
extern int stopWork;

//semafori
//sems[0] javlja kada je kraj rada (koristi se samo za helpSubscribers tredove)
//sems[1-5] javlju da je ubacen uclanak u red
//sems[6-10] javljaju da je stigao novi sabskrajber
extern HANDLE sems[11];

//kriticne sekcije
//stopWorkCS za mjenjanje i citanje zajednicke promjenljive stopWork
//lDictionary za rad sa rjecnicima u kojema se nalaze liste klijentskih(sabskrajberi) soketa
//qDictionary za rad sa rjecnicima u kojema se nalaze redovi sa clancima
//pubList za rad sa listom klijentskih(pabliseri) soketa
extern CRITICAL_SECTION stopWorkCS, lDictionary[5], qDictionary[5], pubList;
#pragma endregion Externe promjenljive

#pragma region Pomocne funkcije
//funkcija zaduzena za sigurno vadjenje iz reda
article criticalDequeue(char tema);

//funkcija zaduzena za sigurno preuzimanje sledeceg elemnta iz liste
SocketNode* criticalNext(SocketNode* head, char tema);

//funkcija za sigurno preuzimanje promjenjive stopWork
int criticalStopWork();

//vraca naziv teme na osnovu njene brojne protivvrijednosti
const char* giveMeTopic(int c);

//provjera da li postoje konektovani sabskrajberi
int criticalCheckList(char c);

//provjera da li postoji clanaka u redu
int criticalCheckQueue(char c);
#pragma endregion Pomocne funkcije

#pragma region Tred funkcije
//ova funkcija je zaduzena za "slusanje" sabskrajbera, kreiranje soketa, primanje tema i smjestanje soketa u odgovarajucu listu
//soketa na osnovu teme
DWORD WINAPI listenForSubscribers(LPVOID lpParam);

//funkcija zaduzena za isporucivanje clanaka klijentima koji su sabskrajbovani na tema za koju je tred zaduzen i za sabskrajbovanje klijenata
//na druge teme
//parametar je tema za koju je tred zaduzen
DWORD WINAPI helpSubscribers(LPVOID lpParam);

//ova funkcija je zaduzena za "slusanje" pablisera, kreiranje soketa i smjestanje soketa u u listu pablisera
//i za prijem clanaka od pablisera koje onda smjesta u odgovarajuci red u QueueDictionary
DWORD WINAPI helpPublishers(LPVOID lpParam);
#pragma endregion Tred funkcije


