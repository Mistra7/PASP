#include <ws2tcpip.h>
#include "Dictionary.h"

int main()
{
	add(0x01, NULL);
	add(0x01, NULL);
	add(0x02, NULL);
	add(0x03, NULL);

	return 0;
}