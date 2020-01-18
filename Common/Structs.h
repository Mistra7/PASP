#pragma once

#define AUTHOR_LEN 21
#define TEXT_LEN 101

typedef struct article {
	char topic; //0x01, 0x02, 0x03, 0x04, 0x05 (games, tehnology, memes, celebrities, sport, respektivno)
	char authorName[AUTHOR_LEN];
	char text[TEXT_LEN];
}article;