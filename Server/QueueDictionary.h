#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Queue.h"

#define HASHSIZE2 100

struct nlist2 {
	char key;
	queue* articleQueue;
	struct nlist2* next;
};

static struct nlist2* hashTab2[HASHSIZE2];

unsigned hash2(char s);
struct nlist2* lookup2(char s);
struct nlist2* install2(char key);
void enqueue(char key, article value);
article* dequeue(char key, article value);
#pragma once
