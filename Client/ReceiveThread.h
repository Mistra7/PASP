#pragma once
#include <conio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Common/List.h"
#include "../Common/Structs.h"

#define DEFAULT_BUFLEN 512

extern HANDLE hEnableReceive;
extern CRITICAL_SECTION cs;
extern node* subbedTopics;

DWORD WINAPI receive(LPVOID lpParam);

void Select(SOCKET, bool);