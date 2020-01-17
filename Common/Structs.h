#pragma once
typedef struct article {
	char topic; //0x01, 0x02, 0x03, 0x04, 0x05 (games, tehnology, memes, celebrities, sport, respektivno)
	char authorName[20];
	char text[101];
}article;