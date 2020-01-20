#pragma once
#include "Core.h"

extern CRITICAL_SECTION list_cs, exit_cs;
extern node* posts;

struct threadParam{
	SOCKET socket;
	user_type type;
	bool* exit;
};

DWORD WINAPI receive(LPVOID lpParam);
