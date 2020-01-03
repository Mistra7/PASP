#include <ws2tcpip.h>
#include "Dictionary.h"
#include "Queue.h"

int main()
{
	add(0x01, NULL);
	add(0x01, NULL);
	add(0x02, NULL);
	add(0x03, NULL);

	/*queue q;
	InitQ(&q);

	Add(&q, 5);
	Add(&q, 3);
	Add(&q, 8);
	Add(&q, 12);
	Add(&q, 2);

	printf("%d\n", Remove(&q));
	printf("%d\n", Remove(&q));
	printf("%d\n", Remove(&q));
	Add(&q, 4);
	printf("%d\n", Remove(&q));
	printf("%d\n", Remove(&q));
	printf("%d\n", Remove(&q));
	printf("%d\n", Remove(&q));*/

	return 0;
}