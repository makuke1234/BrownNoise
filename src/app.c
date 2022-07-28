#include "app.h"
#include "resource.h"
#include "error.h"

bool bn_init(bndata_t * restrict This, int argc, char ** argv)
{
	assert(This != NULL);
	assert(argc > 0);
	assert(argv != NULL);
	assert(argv[0] != NULL);

	*This = (bndata_t){
		.init     = false,
		.dpi      = USER_DEFAULT_SCREEN_DPI,
		.hwnd     = NULL,
		.normFont = NULL,
		.ui       = {
			.circuitType = NULL
		}
	};

	SetProcessDPIAware();

	INITCOMMONCONTROLSEX icex = {
		.dwICC  = ICC_WIN95_CLASSES,
		.dwSize = sizeof(INITCOMMONCONTROLSEX)
	};
	InitCommonControlsEx(&icex);

	if (!bn_regClassBg(BROWN_NOISE_CLASS, &bn_proc, RGB(240, 240, 240)))
	{
		ePrint("Error initializing windows class!");
		return false;
	}

	This->hwnd = CreateWindowExW(
		0,
		BROWN_NOISE_CLASS, BROWN_NOISE_TITLE,
		WS_OVERLAPPEDWINDOW & (DWORD)~( WS_MAXIMIZEBOX ),
		CW_USEDEFAULT, CW_USEDEFAULT,
		400, 300,
		NULL, NULL, GetModuleHandleW(NULL), This
	);
	if (This->hwnd == NULL)
	{
		ePrint("Error creating window!");
		return false;
	}

	ShowWindow(This->hwnd, SW_SHOW);
	UpdateWindow(This->hwnd);

	This->init = true;
	return true;
}
void bn_loop(bndata_t * restrict This)
{
	assert(This != NULL);
	assert(This->init);

	MSG msg = { 0 };
	BOOL bret;
	while ((bret = GetMessageW(&msg, NULL, 0, 0)) != 0)
	{
		if (bret == -1)
		{
			return;
		}
		else if (!IsDialogMessageW(This->hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
}
void bn_free(bndata_t * restrict This)
{
	This->init = false;
}

LRESULT CALLBACK bn_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hwnd, msg, wp, lp);
	}

	return 0;
}
