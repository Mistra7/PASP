struct pasp {
	char funCode;//ack(0x01), publish(0x02), subscribe(0x03), article(0x04), clientshutdown(0x05), servershutdown(0x06), error(+0x10);
	char datalenght;
	char* data;
};

struct article {
	char topic; //0x01, 0x02, 0x03, 0x04, 0x05
	char authorName[20]; //'\0'
	char text[101];
};