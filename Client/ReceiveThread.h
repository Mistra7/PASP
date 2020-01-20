#pragma once
#include <conio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Common/List.h"
#include "../Common/Structs.h"

#define DEFAULT_BUFLEN 512

enum user_type { PUBLISHER, SUBSCRIBER };

extern CRITICAL_SECTION list_cs, exit_cs;
extern node* posts;

struct threadParam{
	SOCKET socket;
	user_type type;
	bool* exit;
};

DWORD WINAPI receive(LPVOID lpParam);

void Select(SOCKET, bool);