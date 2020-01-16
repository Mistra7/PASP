#pragma once
#include <ws2tcpip.h>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "QueueDictionary.h"
#include "Dictionary.h"

extern int stopWork;
//semafori, 5, po jedan za svaki red
extern HANDLE sems[6];
extern CRITICAL_SECTION stopWorkCS;

//ova funkcija je zaduzena za "slusanje" sabskrajbera, kreiranje soketa, primanje prve teme i smjestanje soketa u odgovarajucu listu
//soketa na osnovu teme
DWORD WINAPI listenForSubscribers(LPVOID lpParam);

//funkcija zaduzena za isporucivanje clanaka klijentima koji su sabskrajbovani na tema za koju je tred zaduzen i za sabskrajbovanje klijenata
//na druge teme
//parametar je tema za koju je tred zaduzen
DWORD WINAPI helpSubscribers(LPVOID lpParam);

//ova funkcija je zaduzena za prijem clanaka od pablisera koje onda smjesta u odgovarajuci red u QueueDictionary
DWORD WINAPI helpPublishers(LPVOID lpParam);

//ova funkcija je zaduzena za "slusanje" pablisera, kreiranje soketa i smjestanje soketa u u listu pablisera
DWORD WINAPI listenForPublishers(LPVOID lpParam);



