#include "QueueDictionary.h"

unsigned hash2(char s)
{
	unsigned hashval;
	hashval = (int)pow(s, 2) + (int)(31 * HASHSIZE2);
	return hashval % HASHSIZE2;
}

struct nlist2* lookup2(char s)
{
	struct nlist2* returnValue;

	for (returnValue = hashTab2[hash2(s)]; returnValue != NULL; returnValue = returnValue->next)
	{
		if (returnValue->key == s)
		{
			return returnValue;
		}
	}

	return NULL;
}

struct nlist2* install2(char key)
{
	struct nlist2* np;
	unsigned hashval;
	if ((np = lookup2(key)) == NULL)
	{
		np = (struct nlist2*)malloc(sizeof(*np));
		if (np == NULL)
			return NULL;
		np->key = key;
		np->articleQueue = NULL;
		InitQ(np->articleQueue);
		hashval = hash2(key);
		np->next = hashTab2[hashval];
		hashTab2[hashval] = np;
	}
	return np;
}

void enqueue(char key, article value)
{
	struct nlist2* np;
	if ((np = lookup2(key)) == NULL)
		np = install2(key);
	add(np->articleQueue, value);
}

article* dequeue(char key, article value)
{
	struct nlist2* np;
	if ((np = lookup2(key)) != NULL)
		return remove((np->articleQueue));
	return NULL;
}
