#pragma once
#include <conio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Common/List.h"
#include "../Common/Structs.h"

#define DEFAULT_BUFLEN 512

enum user_type { PUBLISHER, SUBSCRIBER };

void Select(SOCKET, bool);

char chooseTopic();

void ToContinue();

extern node* posts;
extern node* subbedTopics;