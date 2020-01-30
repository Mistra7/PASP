#include <string.h>
#include <math.h>
#include "SocketList.h"

#define HASHSIZE 100

//opis: key value par
struct nlist {
	char key;
	SocketNode* clientSockets;
	struct nlist* next;
};

//opis: hash tabela
static struct nlist* hashTab[HASHSIZE];

#pragma region funkcije za rad sa rjecnikom
/*
opis: stvaranje hash vrijednosti
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: hash vrijendost
*/
unsigned hash(char s);

/*
opis:gleda da li za zadatu vrijednost postoji key, value par u rjecniku
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: key value par
*/
struct nlist* lookup(char s);

/*
opis: stvaranje novog key, value para u rjecniku (vrsi provjeru da li vec postoji)
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: key value par
*/
struct nlist* install(char key);

/*
opis: dodaje novi soket u odgovarajucu listu (vrsi provjeri da li vec postoji key value par, ako ne, pravi novi)
parametar: karakter na osnovu kojeg se kreira hash, soket koji ubacujemo
povratna vrijednost: nema
*/
void add(char key, SOCKET value);

/*
opis: izbacuje soket iz liste (oslobadja memoriju)
parametar: karakter na osnovu kojeg se kreira hash, soket koji izbacujemo
povratna vrijednost: nema
*/
void removeValue(char key, SOCKET value);

/*
opis: vraca prvi element u listi soketa 
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: prvi element u listi
*/
SocketNode* getAllSockets(char key);

/*
opis: unistava key value par u rjecniku
parametar: karakter na osnovu kojeg se kreira hash
povratna vrijednost: nema
*/
void destroyKeyValueLD(char key);
#pragma endregion funkcije za rad sa rjecnikom
