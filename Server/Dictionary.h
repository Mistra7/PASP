#include <string.h>
#include <math.h>
#include "SocketList.h"

#define HASHSIZE 100

struct nlist {
	char key;
	SocketNode* clientSockets;
	struct nlist* next;
};

static struct nlist* hashTab[HASHSIZE];

unsigned hash(char s);
struct nlist* lookup(char s);
struct nlist* install(char key);
void add(char key, SOCKET value);
void removeValue(char key, SOCKET value);
SocketNode* getAllSockets(char key);
