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

#pragma region funkcije za rad sa rjecnikom
//stvaranje hash vrijednosti
unsigned hash(char s);
//gleda da li za zadatu vrijednost postoji key, value par u rjecniku
struct nlist* lookup(char s);
//stvaranje novog key, value para u rjecniku (vrsi provjeru da li vec postoji)
struct nlist* install(char key);
//dodaje novi soket u odgovarajucu listu (vrsi provjeri da li vec postoji key value par, ako ne, pravi novi)
void add(char key, SOCKET value);
//izbacuje soket iz liste (oslobadja memoriju)
void removeValue(char key, SOCKET value);
//vraca listu 
SocketNode* getAllSockets(char key);
#pragma endregion funkcije za rad sa rjecnikom
