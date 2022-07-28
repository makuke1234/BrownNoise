#ifndef APP_H
#define APP_H

#include "winapi.h"

enum circuitType
{
	circtype_norm,
	circtype_diff,
	circtype_integrator
};

#define DROP1_SIZE 3


typedef struct bnui
{
	HBITMAP bmps[DROP1_SIZE];
	HBITMAP bmp;
	
	HWND circuitTypeHandle;
	enum circuitType circuitTypeIdx;

} bnui_t;

typedef struct bndata
{
	bool init;
	int dpi;
	HINSTANCE hinst;

	HWND hwnd;
	HFONT normFont;

	bnui_t ui;

} bndata_t;

bool bn_init(bndata_t * restrict This, HINSTANCE hinst);
void bn_loop(bndata_t * restrict This);
void bn_free(bndata_t * restrict This);

HWND bn_createDrop(
	int x, int y,
	int cx, int cy,
	HWND parent,
	HMENU hmenu,
	const wchar ** restrict options, usize numopt, usize defopt
);
usize bn_getDropSel(HWND drop);
void bn_setFont(HWND hwnd, HFONT hfont);


LRESULT CALLBACK bn_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void bn_createUI(bndata_t * restrict This);
void bn_size(bndata_t * restrict This);
void bn_paint(bndata_t * restrict This, HDC hdc);
void bn_command(bndata_t * restrict This, WPARAM wp, LPARAM lp);
void bn_update(bndata_t * restrict This);

#endif
