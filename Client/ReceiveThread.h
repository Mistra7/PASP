#pragma once
#include <conio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512

extern HANDLE hEnableReceive;
extern CRITICAL_SECTION cs;

DWORD WINAPI receive(LPVOID lpParam);

void Select(SOCKET, bool);