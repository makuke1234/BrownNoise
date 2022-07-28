#include "app.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpCmdArgs, int nCmdShow)
{
	bnUNUSED(hPrevInst);
	bnUNUSED(lpCmdArgs);
	bnUNUSED(nCmdShow);

	bndata_t data = { 0 };
	if (!bn_init(&data, hInst))
	{
		return -1;
	}

	bn_loop(&data);

	bn_free(&data);

	return 0;
}
