#pragma once
#include "Core.h"

//Kriticne sekcije za rukovanje sa listom post-ova i bool-om za gasenje klijenta
extern CRITICAL_SECTION list_cs, exit_cs;

/*
Struktura koja predstavljam parametar za tred koji prima poruke
od servera. Sastoji se od soketa na koji stizu poruke, tipa korisnika
i bool-a za gasenje klijenta
*/
struct threadParam{
	SOCKET socket;
	user_type type;
	bool* exit;
};

/*
Opis: Tred za prijem post-ova i signala o prekidu rada servera
Param: Struktura threadParam
Povratni: 0
*/
DWORD WINAPI receive(LPVOID lpParam);
