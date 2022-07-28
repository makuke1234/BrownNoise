#ifndef APP_H
#define APP_H

#include "winapi.h"

typedef struct bnui
{
	HWND circuitType;

} bnui_t;

typedef struct bndata
{
	bool init;
	int dpi;

	HWND hwnd;
	HFONT normFont;

	bnui_t ui;

} bndata_t;

bool bn_init(bndata_t * restrict This, int argc, char ** argv);
void bn_loop(bndata_t * restrict This);
void bn_free(bndata_t * restrict This);

LRESULT CALLBACK bn_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

#endif
