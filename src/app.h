#ifndef APP_H
#define APP_H

#include "winapi.h"
#include "winapihelper.h"

enum circuitType
{
	circtype_norm,
	circtype_diff,
	circtype_integrator,

	circtype_default = circtype_norm
};
#define DROPTYPE_SIZE 3


enum nUnitType
{
	nutype_nv_rthz,
	nutype_uv_rms,

	nutype_default = nutype_uv_rms
};
#define DROPNOISEU_SIZE 2

#define NOISE_CONSTANT 1.57
#define BOLTZMANN_CONSTANT 1.380649e-23


enum bwUnitType
{
	bwutype_hz,
	bwutype_khz,
	bwutype_mhz,
	bwutype_ghz,

	bwutype_default = bwutype_khz
};
#define DROPBWUNIT_SIZE 4


enum tUnitType
{
	tutype_celsius,
	tutype_kelvin,
	tutype_fahrenheit,

	tutype_default = tutype_celsius
};
#define DROPTUNIT_SIZE 3


#define MAX_RESULT 100

#define DEF_NTEXT  L""
#define DEF_BWTEXT L"20"
#define DEF_TTEXT  L"25"
#define DEF_DNTEXT L""

typedef struct bnui
{
	HBITMAP bmps[DROPTYPE_SIZE];
	HBITMAP bmp;
	
	HWND circuitTypeHandle;
	enum circuitType circuitTypeIdx;

	HWND noiseTextHandle, noiseUnitHandle;
	enum nUnitType noiseUnitIdx;
	double noiseValue;

	HWND bwTextHandle, bwUnitHandle;
	enum bwUnitType bwUnitIdx;
	double bwValue;

	HWND tempTextHandle, tempUnitHandle;
	enum tUnitType tempUnitIdx;
	double tempValue;

	HWND desiredNTextHandle, desiredNUnitHandle;
	enum nUnitType desiredNUnitIdx;
	double desiredNValue;

	HWND resetBtn;

} bnui_t;

typedef struct bndata
{
	bool init;
	int dpi;
	HINSTANCE hinst;

	HWND hwnd;
	HFONT normFont;

	bnui_t ui;
	double impedance;

} bndata_t;

bool bn_init(bndata_t * restrict This, HINSTANCE hinst);
void bn_loop(bndata_t * restrict This);
void bn_free(bndata_t * restrict This);

HWND bn_createDrop(
	int x, int y,
	int cx, int cy,
	HWND parent, HMENU hmenu,
	const wchar ** restrict options, usize numopt
);
HWND bn_createDropDef(
	int x, int y,
	int cx, int cy,
	HWND parent, HMENU hmenu,
	const wchar ** restrict options, usize numopt, usize defopt
);
HWND bn_createNumText(
	int x, int y,
	int cx, int cy,
	HWND parent, HMENU hmenu
);
usize bn_getDropSel(HWND drop);
void bn_setDropSel(HWND drop, usize sel);
void bn_setFont(HWND hwnd, HFONT hfont);
bool bn_getBorder(HWND hwnd, int * restrict bx, int * restrict by);
void bn_setWindowSize(HWND hwnd, int cx, int cy);
bool bn_isnan(double x);
double bn_noiseVolts(double value, double bandwidth, enum nUnitType nUnit);
HBITMAP bn_loadPNG(HINSTANCE hinst, LPCWSTR rscName);

LRESULT CALLBACK bn_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void bn_createUI(bndata_t * restrict This);
void bn_size(bndata_t * restrict This);
void bn_paint(bndata_t * restrict This, HDC hdc);
void bn_command(bndata_t * restrict This, WPARAM wp, LPARAM lp);
void bn_update(bndata_t * restrict This);
void bn_setDefaults(bndata_t * restrict This);

#endif
