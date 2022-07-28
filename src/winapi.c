#include "winapi.h"

#pragma GCC diagnostic ignored "-Wcast-function-type"


bool bn_regClass(const wchar * restrict className, WNDPROC winProc)
{
	return bn_regClassBg(className, winProc, GetSysColor(COLOR_3DFACE));
}
bool bn_regClassBg(const wchar * restrict className, WNDPROC winProc, COLORREF rgbColor)
{
	WNDCLASSEXW wc   = { 0 };
	wc.cbSize        = sizeof wc;
	wc.lpfnWndProc   = winProc;
	wc.hInstance     = GetModuleHandleW(NULL);
	wc.hIcon         = LoadIconW(wc.hInstance, MAKEINTRESOURCEW(IDI_APPLICATION));
	wc.hCursor       = LoadCursorW(wc.hInstance, MAKEINTRESOURCEW(IDC_ARROW));
	if (wc.hCursor == NULL)
	{
		wc.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
	}
	wc.hbrBackground = CreateSolidBrush(rgbColor);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = className;
	wc.hIconSm       = wc.hIcon;

	return RegisterClassExW(&wc) != 0;
}

static int s_defaultDpi = USER_DEFAULT_SCREEN_DPI;

int bn_dpi(HWND hwnd)
{
	if (hwnd != NULL)
	{
		const int dpi = (int)ms_GetDpiForWindow(hwnd);
		if (dpi)
		{
			s_defaultDpi = dpi;
			return dpi;
		}
	}
	else
	{
		const int dpi = (int)ms_GetDpiForSystem();
		if (dpi)
		{
			s_defaultDpi = dpi;
			return dpi;
		}
	}

	HDC hdc = GetDC(hwnd);
	if (hdc != NULL)
	{
		const int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
		ReleaseDC(hwnd, hdc);
		s_defaultDpi = dpi;
		return dpi;
	}
	else
	{
		return USER_DEFAULT_SCREEN_DPI;
	}
}

int bn_hdpi(HWND hwnd, int size)
{
	return bn_cdpi(bn_dpi(hwnd), size);
}
int bn_cdpi(int dpi, int size)
{
	return MulDiv(size, dpi, 96);
}
int bn_fdpi(int dpi, int size)
{
	return -MulDiv(size, dpi, 72);
}
int bn_defcdpi(int size)
{
	return bn_cdpi(s_defaultDpi, size);
}
int bn_deffdpi(int size)
{
	return bn_fdpi(s_defaultDpi, size);
}

static pfnGetDpiForSystem_t pfnGetDpiForSystem = NULL;
static pfnGetDpiForWindow_t pfnGetDpiForWindow = NULL;

bool ms_initSymbols(void)
{
	static bool s_init = false, s_success = false;
	
	if (s_init)
	{
		return s_success;
	}

	HMODULE user32 = GetModuleHandleW(L"user32");

	pfnGetDpiForSystem = FARPROC_cast(pfnGetDpiForSystem_t, GetProcAddress(user32, "GetDpiForSystem"));
	pfnGetDpiForWindow = FARPROC_cast(pfnGetDpiForWindow_t, GetProcAddress(user32, "GetDpiForWindow"));

	s_init    = true;
	s_success = (pfnGetDpiForSystem != NULL) && (pfnGetDpiForWindow != NULL);

	return s_success;
}

UINT ms_GetDpiForWindow(HWND hwnd)
{
	ms_initSymbols();
	return (pfnGetDpiForWindow != NULL) ? pfnGetDpiForWindow(hwnd) : 0;
}
UINT ms_GetDpiForSystem(void)
{
	ms_initSymbols();
	return (pfnGetDpiForSystem != NULL) ? pfnGetDpiForSystem() : 0;
}

#pragma GCC diagnostic pop

