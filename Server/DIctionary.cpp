#include "Dictionary.h"

unsigned hash(char s)
{
	unsigned hashval;
	hashval = (int)pow(s,2) + (int)(31 * HASHSIZE);
	return hashval % HASHSIZE;
}

struct nlist* lookup(char s)
{
	struct nlist *returnValue;

	returnValue = hashTab[hash(s)];
	if (returnValue != NULL)
	{
		if (returnValue->key == s)
		{
			return returnValue;
		}
	}	

	return NULL;
}

struct nlist* install(char key)
{
	struct nlist* np;
	unsigned hashval;
	if ((np = lookup(key)) == NULL)
	{
		np = (struct nlist*) malloc(sizeof(*np));
		if (np == NULL)
			return NULL;
		np->key = key;
		np->clientSockets = NULL;
		hashval = hash(key);
		np->next = hashTab[hashval];
		hashTab[hashval] = np;
	}
	return np;
}

void add(char key, SOCKET value)
{
	struct nlist* np;
	if ((np = lookup(key)) == NULL)
		np = install(key);
	addSocket(&(np->clientSockets), value);	
}

void removeValue(char key, SOCKET value)
{
	struct nlist* np;
	if ((np = lookup(key)) != NULL)
		deleteSocket(&(np->clientSockets), value);
}

SocketNode* getAllSockets(char key)
{
	struct nlist* np;
	if ((np = lookup(key)) != NULL)
		return np->clientSockets;
	return NULL;
}

void destroyKeyValueLD(char key)
{
	struct nlist* np = lookup(key);

	if (np == NULL)
		return;

	SocketNode* current = np->clientSockets;
	deleteList(&(np->clientSockets));	

	free(np);
	
}

//void destroyDictionary(char key)
//{
//	struct nlist* np = lookup(key);
//
//	if (np == NULL)
//		return;
//
//}