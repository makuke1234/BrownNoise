#ifndef WINAPIHELPER_H
#define WINAPIHELPER_H

#include "winapi.h"

#define bnUNUSED(x) ((void)x)
#define FARPROC_cast(type, func) (type)func

bool bn_regClass(const wchar * restrict className, WNDPROC winProc);
bool bn_regClassBg(const wchar * restrict className, WNDPROC winProc, COLORREF rgbColor);

int bn_dpi(HWND hwnd);
int bn_hdpi(HWND hwnd, int size);
int bn_cdpi(int dpi, int size);
int bn_fdpi(int dpi, int size);
int bn_defcdpi(int size);
int bn_deffdpi(int size);


typedef UINT (WINAPI * pfnGetDpiForSystem_t)(void);
typedef UINT (WINAPI * pfnGetDpiForWindow_t)(HWND hwnd);

bool ms_initSymbols(void);

UINT ms_GetDpiForWindow(HWND hwnd);
UINT ms_GetDpiForSystem(void);

#endif
