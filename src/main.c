#include "app.h"

int main(int argc, char * argv[])
{
	bndata_t data = { 0 };
	if (!bn_init(&data, argc, argv))
	{
		return -1;
	}

	bn_loop(&data);

	bn_free(&data);

	return 0;
}
