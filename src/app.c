#include "app.h"
#include "resource.h"
#include "error.h"
#include "wicwrapper.h"

static struct
{
	const wchar * droptype[DROPTYPE_SIZE];
	const wchar * dropnoiseu[DROPNOISEU_SIZE];
	const wchar * dropbwunit[DROPBWUNIT_SIZE];
	const wchar * droptunit[DROPTUNIT_SIZE];
	const wchar * dropinpunit[DROPINPUNIT_SIZE];
	const wchar * dropsnrunit[DROPSNRUNIT_SIZE];

} s_def = {
	.droptype = {
		L"Resistor feedback",
		L"Differentiator",
		L"Integrator"
	},
	.dropnoiseu = {
		L"nV/\u221AHz",
		L"\u00B5V RMS",
		L"dBV"
	},
	.dropbwunit = {
		L"Hz",
		L"KHz",
		L"MHz",
		L"GHz"
	},
	.droptunit = {
		L"\u00B0C",
		L"Kelvin",
		L"\u00B0F"
	},
	.dropinpunit = {
		L"\u00B5V",
		L"mV",
		L"V",
		L"dBV",
		L"dBu",
		L"dBmV",
		L"dB\u00B5V"
	},
	.dropsnrunit = {
		L"dB",
		L"Linear"
	}
};

bool bn_init(bndata_t * restrict This, HINSTANCE hInst)
{
	assert(This != NULL);
	assert(hInst != NULL);

	*This = (bndata_t){
		.init  = false,
		.dpi   = USER_DEFAULT_SCREEN_DPI,
		.hinst = hInst,
		
		.hwnd     = NULL,
		.normFont = NULL,
		
		.ui = {
			.bmps = { NULL, NULL, NULL },
			.bmp  = NULL,

			.circuitTypeHandle = NULL,
			.circuitTypeIdx    = circtype_default,
			
			.noiseTextHandle = NULL,
			.noiseUnitHandle = NULL,
			.noiseUnitIdx    = nutype_default,
			.noiseValue      = 0.0,

			.bwTextHandle = NULL,
			.bwUnitHandle = NULL,
			.bwUnitIdx    = bwutype_default,
			.bwValue      = 0.0,

			.tempTextHandle = NULL,
			.tempUnitHandle = NULL,
			.tempUnitIdx    = tutype_default,
			.tempValue      = 0.0,
			
			.desiredNTextHandle = NULL,
			.desiredNUnitHandle = NULL,
			.desiredNUnitIdx    = nutype_default,
			.desiredNValue      = 0.0,

			.resetBtn = NULL,


			.inpTextHandle = NULL,
			.inpUnitHandle = NULL,
			.inpUnitIdx    = inputype_default,
			.inpValue      = 0.0,

			.snrTextHandle = NULL,
			.snrUnitHandle = NULL,
			.snrUnitIdx    = snrtype_default,
			.snrValue      = 0.0
		},

		.impedance    = 0.0,
		.snrImpedance = 0.0,

		.snrFromNoise = 0.0,
		.noiseFromSnr = 0.0
	};

	SetProcessDPIAware();

	INITCOMMONCONTROLSEX icex = {
		.dwICC  = ICC_WIN95_CLASSES,
		.dwSize = sizeof(INITCOMMONCONTROLSEX)
	};
	InitCommonControlsEx(&icex);

	if (!bn_regClass(BROWN_NOISE_CLASS, &bn_proc))
	{
		ePrint("Error initializing windows class!");
		return false;
	}

	This->hwnd = CreateWindowExW(
		0,
		BROWN_NOISE_CLASS, BROWN_NOISE_TITLE,
		WS_OVERLAPPEDWINDOW & (DWORD)~( WS_MAXIMIZEBOX ),
		CW_USEDEFAULT, CW_USEDEFAULT,
		SIZE_X, SIZE_Y,
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

HWND bn_createDrop(
	int x, int y,
	int cx, int cy,
	HWND parent, HMENU hmenu,
	const wchar ** restrict options, usize numopt
)
{
	assert(parent != NULL);
	assert(options != NULL);
	assert(numopt > 0);
	assert(((WPARAM)numopt) != ((WPARAM)-1));
	assert(options[0] != NULL);

	HWND box = CreateWindowExW(
		0,
		L"combobox", NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP,
		x,  y,
		cx, cy,
		parent,
		hmenu,
		(HINSTANCE)GetWindowLongPtrW(parent, GWLP_HINSTANCE),
		NULL
	);
	if (box == NULL)
	{
		return NULL;
	}

	// Add items to dropdown
	for (usize i = 0; i < numopt; ++i)
	{
		assert(options[i] != NULL);
		SendMessageW(box, CB_ADDSTRING, 0, (LPARAM)options[i]);
	}

	return box;
}
HWND bn_createDropDef(
	int x, int y,
	int cx, int cy,
	HWND parent, HMENU hmenu,
	const wchar ** restrict options, usize numopt, usize defopt
)
{
	HWND box = bn_createDrop(
		x, y,
		cx, cy,
		parent, hmenu,
		options, numopt
	);
	if (box == NULL)
	{
		return NULL;
	}

	// Set default value
	assert(((WPARAM)defopt) < ((WPARAM)numopt));
	bn_setDropSel(box, defopt);

	return box;
}
HWND bn_createNumText(
	int x, int y,
	int cx, int cy,
	HWND parent, HMENU hmenu
)
{
	HWND box = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		L"edit", L"",
		WS_CHILD | WS_VISIBLE | ES_CENTER | WS_TABSTOP,
		x,  y,
		cx, cy,
		parent,
		(HMENU)hmenu,
		(HINSTANCE)GetWindowLongPtrW(parent, GWLP_HINSTANCE),
		NULL
	);
	return box;
}
usize bn_getDropSel(HWND drop)
{
	return (usize)SendMessageW(drop, CB_GETCURSEL, 0, 0);
}
void bn_setDropSel(HWND drop, usize sel)
{
	const WPARAM wpdefopt = (WPARAM)sel;
	assert(wpdefopt != (WPARAM)-1);
	SendMessageW(drop, CB_SETCURSEL, wpdefopt, 0);
}
void bn_setFont(HWND hwnd, HFONT hfont)
{
	SendMessageW(hwnd, WM_SETFONT, (WPARAM)hfont, FALSE);
}
bool bn_getBorder(HWND hwnd, int * restrict bx, int * restrict by)
{
	RECT rc, rw;
	if (!GetWindowRect(hwnd, &rw) || !GetClientRect(hwnd, &rc))
	{
		*bx = 0;
		*by = 0;
		return false;
	}

	// Calculate border overhang, this function will set the client area
	*bx = (rw.right  - rw.left) - (rc.right  - rc.left);
	*by = (rw.bottom - rw.top)  - (rc.bottom - rc.top);

	return true;
}
void bn_setWindowSize(HWND hwnd, int cx, int cy)
{
	int bx, by;
	bn_getBorder(hwnd, &bx, &by);
	
	SetWindowPos(
		hwnd, NULL,
		0,  0,
		cx + bx, cy + by,
		SWP_NOZORDER | SWP_NOMOVE | SWP_NOOWNERZORDER
	);
}
bool bn_isnan(double x)
{
	return x != x;
}
double bn_noiseVolts(double value, double bandwidth, enum nUnitType nUnit)
{
	switch (nUnit)
	{
	case nutype_nv_rthz:
		// Calculate RMS noise voltage based on approximation, that
		// noise decreases linearly up to 100 Hz and beyond that the response is flat

		return (value * sqrt(bandwidth * NOISE_CONSTANT)) / 1e9;
	case nutype_uv_rms:
		return value / 1e6;
	case nutype_dbv:
		return pow(10.0, value / 20.0);
	default:
		return value;
	}
}


HBITMAP bn_loadPNG(HINSTANCE hinst, LPCWSTR rscName)
{
	return wic_LoadPNG(hinst, rscName);
}

int bn_printImpedance(wchar * arr, usize arrLen, const wchar * pilotText, double impedance)
{
	if (bn_isnan(impedance))
	{
		return swprintf_s(arr, arrLen - 1, L"%sNaN", pilotText);
	}
	else
	{
		// Implement autorange feature
		const wchar * unit = L"\u2126";
		double imp = impedance;
		if (imp >= 2e3)
		{
			imp /= 1e3;
			unit = L"K\u2126";
		}
		else if (imp >= 2e6)
		{
			imp /= 1e6;
			unit = L"M\u2126";
		}
		else if (imp >= 2e9)
		{
			imp /= 1e9;
			unit = L"G\u2126";
		}

		return swprintf_s(arr, arrLen - 1, L"%s%.2f %s", pilotText, imp, unit);
	}
}
int bn_printNoise(wchar * arr, usize arrLen, const wchar * pilotText, double volts, enum nUnitType nUnit)
{
	if (bn_isnan(volts))
	{
		return swprintf_s(arr, arrLen - 1, L"%sNaN", pilotText);
	}
	else
	{
		return swprintf_s(arr, arrLen - 1, L"%s%.2f %s", pilotText, volts, s_def.dropnoiseu[nUnit]);
	}
}
int bn_printSNR(wchar * arr, usize arrLen, const wchar * pilotText, double snr, enum snrUnitType snrUnit)
{
	if (bn_isnan(snr))
	{
		return swprintf_s(arr, arrLen - 1, L"%sNaN", pilotText);
	}
	else
	{
		return swprintf_s(arr, arrLen - 1, L"%s%.2f %s", pilotText, snr, s_def.dropsnrunit[snrUnit]);
	}
}



LRESULT CALLBACK bn_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	bndata_t * restrict This = NULL;

	if (msg == WM_NCCREATE)
	{
		This = ((CREATESTRUCTW *)lp)->lpCreateParams;
		assert(This != NULL);

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)This);
		This->hwnd = hwnd;
	}
	else
	{
		This = (bndata_t *)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	}

	if (This == NULL)
	{
		return DefWindowProcW(hwnd, msg, wp, lp);
	}

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		bn_paint(This, hdc);

		EndPaint(hwnd, &ps);
		break;
	}
	case WM_COMMAND:
		bn_command(This, wp, lp);
		break;
	case WM_SIZE:
		break;
	case WM_SIZING:
	{
		const int olddpi = This->dpi;
		This->dpi = bn_dpi(hwnd);
		if (This->dpi != olddpi)
		{
			bn_size(This);
		}
		else
		{
			// Prevent resizing
			RECT * rc = (RECT *)lp;
			assert(rc != NULL);

			int bx, by;
			bn_getBorder(hwnd, &bx, &by);
			
			// X-direction
			switch (wp)
			{
			case WMSZ_TOPLEFT:
			case WMSZ_LEFT:
			case WMSZ_BOTTOMLEFT:
				rc->left = rc->right - bn_defcdpi(SIZE_X) - bx;
				break;
			case WMSZ_TOPRIGHT:
			case WMSZ_RIGHT:
			case WMSZ_BOTTOMRIGHT:
				rc->right = rc->left + bn_defcdpi(SIZE_X) + bx;
				break;
			}

			// Y-direction
			switch (wp)
			{
			case WMSZ_TOPLEFT:
			case WMSZ_TOP:
			case WMSZ_TOPRIGHT:
				rc->top = rc->bottom - bn_defcdpi(SIZE_Y) - by;
				break;
			case WMSZ_BOTTOMLEFT:
			case WMSZ_BOTTOM:
			case WMSZ_BOTTOMRIGHT:
				rc->bottom = rc->top + bn_defcdpi(SIZE_Y) + by;
				break;
			}
		}
		break;
	}
	case WM_CREATE:
		bn_createUI(This);
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

void bn_createUI(bndata_t * restrict This)
{
	This->dpi = bn_dpi(This->hwnd);
	bn_setWindowSize(This->hwnd, bn_defcdpi(SIZE_X), bn_defcdpi(SIZE_Y));

	This->normFont = CreateFontW(
		bn_deffdpi(11), 0, 0, 0,
		FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		L"Segoe UI"
	);

	This->ui.bmps[circtype_norm]       = bn_loadPNG(This->hinst, MAKEINTRESOURCEW(IDB_NORM));
	This->ui.bmps[circtype_diff]       = bn_loadPNG(This->hinst, MAKEINTRESOURCEW(IDB_DIFF));
	This->ui.bmps[circtype_integrator] = bn_loadPNG(This->hinst, MAKEINTRESOURCEW(IDB_INT));

	assert(This->ui.bmps[circtype_norm]       != NULL);
	assert(This->ui.bmps[circtype_diff]       != NULL);
	assert(This->ui.bmps[circtype_integrator] != NULL);

	This->ui.bmp = This->ui.bmps[This->ui.circuitTypeIdx];

	This->ui.circuitTypeHandle = bn_createDrop(
		bn_defcdpi(DROPTYPE_POS_X), bn_defcdpi(DROPTYPE_POS_Y),
		bn_defcdpi(DROPTYPE_SIZE_X), bn_defcdpi(DROPTYPE_SIZE_Y),
		This->hwnd, (HMENU)IDD_DROPTYPE,
		s_def.droptype, DROPTYPE_SIZE
	);
	bn_setFont(This->ui.circuitTypeHandle, This->normFont);

	This->ui.noiseTextHandle = bn_createNumText(
		bn_defcdpi(NOISE_POS_X),  bn_defcdpi(NOISE_POS_Y),
		bn_defcdpi(NOISE_SIZE_X), bn_defcdpi(NOISE_SIZE_Y),
		This->hwnd, (HMENU)IDT_NOISE
	);
	bn_setFont(This->ui.noiseTextHandle, This->normFont);

	This->ui.noiseUnitHandle = bn_createDrop(
		bn_defcdpi(DROPNOISEU_POS_X),  bn_defcdpi(DROPNOISEU_POS_Y),
		bn_defcdpi(DROPNOISEU_SIZE_X), bn_defcdpi(DROPNOISEU_SIZE_Y),
		This->hwnd, (HMENU)IDD_DROPNOISEU,
		s_def.dropnoiseu, DROPNOISEU_SIZE
	);
	bn_setFont(This->ui.noiseUnitHandle, This->normFont);

	This->ui.bwTextHandle = bn_createNumText(
		bn_defcdpi(BANDWIDTH_POS_X),  bn_defcdpi(BANDWIDTH_POS_Y),
		bn_defcdpi(BANDWIDTH_SIZE_X), bn_defcdpi(BANDWIDTH_SIZE_Y),
		This->hwnd, (HMENU)IDT_BANDWIDTH
	);
	bn_setFont(This->ui.bwTextHandle, This->normFont);

	This->ui.bwUnitHandle = bn_createDrop(
		bn_defcdpi(DROPBWUNIT_POS_X),  bn_defcdpi(DROPBWUNIT_POS_Y),
		bn_defcdpi(DROPBWUNIT_SIZE_X), bn_defcdpi(DROPBWUNIT_SIZE_Y),
		This->hwnd, (HMENU)IDD_DROPBWUNIT,
		s_def.dropbwunit, DROPBWUNIT_SIZE
	);
	bn_setFont(This->ui.bwUnitHandle, This->normFont);

	This->ui.tempTextHandle = bn_createNumText(
		bn_defcdpi(TEMP_POS_X),  bn_defcdpi(TEMP_POS_Y),
		bn_defcdpi(TEMP_SIZE_X), bn_defcdpi(TEMP_SIZE_Y),
		This->hwnd, (HMENU)IDT_TEMP
	);
	bn_setFont(This->ui.tempTextHandle, This->normFont);

	This->ui.tempUnitHandle = bn_createDrop(
		bn_defcdpi(DROPTUNIT_POS_X),  bn_defcdpi(DROPTUNIT_POS_Y),
		bn_defcdpi(DROPTUNIT_SIZE_X), bn_defcdpi(DROPTUNIT_SIZE_Y),
		This->hwnd, (HMENU)IDD_DROPTUNIT,
		s_def.droptunit, DROPTUNIT_SIZE
	);
	bn_setFont(This->ui.tempUnitHandle, This->normFont);

	This->ui.desiredNTextHandle = bn_createNumText(
		bn_defcdpi(DESIREDN_POS_X),  bn_defcdpi(DESIREDN_POS_Y),
		bn_defcdpi(DESIREDN_SIZE_X), bn_defcdpi(DESIREDN_SIZE_Y),
		This->hwnd, (HMENU)IDT_DESIREDN
	);
	bn_setFont(This->ui.desiredNTextHandle, This->normFont);

	This->ui.desiredNUnitHandle = bn_createDrop(
		bn_defcdpi(DROPDESIREDN_POS_X),  bn_defcdpi(DROPDESIREDN_POS_Y),
		bn_defcdpi(DROPDESIREDN_SIZE_X), bn_defcdpi(DROPDESIREDN_SIZE_Y),
		This->hwnd, (HMENU)IDD_DROPDESIREDN,
		s_def.dropnoiseu, DROPNOISEU_SIZE
	);
	bn_setFont(This->ui.desiredNUnitHandle, This->normFont);


	This->ui.inpTextHandle = bn_createNumText(
		bn_defcdpi(INP_POS_X),  bn_defcdpi(INP_POS_Y),
		bn_defcdpi(INP_SIZE_X), bn_defcdpi(INP_SIZE_Y),
		This->hwnd, (HMENU)IDT_INP
	);
	bn_setFont(This->ui.inpTextHandle, This->normFont);

	This->ui.inpUnitHandle = bn_createDrop(
		bn_defcdpi(DROPINP_POS_X),  bn_defcdpi(DROPINP_POS_Y),
		bn_defcdpi(DROPINP_SIZE_X), bn_defcdpi(DROPINP_SIZE_Y),
		This->hwnd, (HMENU)IDD_DROPINP,
		s_def.dropinpunit, DROPINPUNIT_SIZE
	);
	bn_setFont(This->ui.inpUnitHandle, This->normFont);

	This->ui.snrTextHandle = bn_createNumText(
		bn_defcdpi(SNR_POS_X),  bn_defcdpi(SNR_POS_Y),
		bn_defcdpi(SNR_SIZE_X), bn_defcdpi(SNR_SIZE_Y),
		This->hwnd, (HMENU)IDT_SNR
	);
	bn_setFont(This->ui.snrTextHandle, This->normFont);

	This->ui.snrUnitHandle = bn_createDrop(
		bn_defcdpi(DROPSNR_POS_X),  bn_defcdpi(DROPSNR_POS_Y),
		bn_defcdpi(DROPSNR_SIZE_X), bn_defcdpi(DROPSNR_SIZE_Y),
		This->hwnd, (HMENU)IDD_DROPSNR,
		s_def.dropsnrunit, DROPSNRUNIT_SIZE
	);
	bn_setFont(This->ui.snrUnitHandle, This->normFont);



	This->ui.resetBtn = CreateWindowExW(
		0,
		L"button",
		L"Reset",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		bn_defcdpi(RESETBTN_POS_X),  bn_defcdpi(RESETBTN_POS_Y),
		bn_defcdpi(RESETBTN_SIZE_X), bn_defcdpi(RESETBTN_SIZE_Y),
		This->hwnd, (HMENU)IDM_RESET,
		This->hinst, NULL
	);
	bn_setFont(This->ui.resetBtn, This->normFont);


	bn_setDefaults(This);
}
void bn_size(bndata_t * restrict This)
{
	bn_setWindowSize(This->hwnd, bn_defcdpi(SIZE_X), bn_defcdpi(SIZE_Y));

	SetWindowPos(
		This->ui.circuitTypeHandle, NULL,
		bn_defcdpi(DROPTYPE_POS_X),  bn_defcdpi(DROPTYPE_POS_Y),
		bn_defcdpi(DROPTYPE_SIZE_X), bn_defcdpi(DROPTYPE_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);

	SetWindowPos(
		This->ui.noiseTextHandle, NULL,
		bn_defcdpi(NOISE_POS_X),  bn_defcdpi(NOISE_POS_Y),
		bn_defcdpi(NOISE_SIZE_X), bn_defcdpi(NOISE_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);
	SetWindowPos(
		This->ui.noiseUnitHandle, NULL,
		bn_defcdpi(DROPNOISEU_POS_X),  bn_defcdpi(DROPNOISEU_POS_Y),
		bn_defcdpi(DROPNOISEU_SIZE_X), bn_defcdpi(DROPNOISEU_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);

	SetWindowPos(
		This->ui.bwTextHandle, NULL,
		bn_defcdpi(BANDWIDTH_POS_X),  bn_defcdpi(BANDWIDTH_POS_Y),
		bn_defcdpi(BANDWIDTH_SIZE_X), bn_defcdpi(BANDWIDTH_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);
	SetWindowPos(
		This->ui.bwUnitHandle, NULL,
		bn_defcdpi(DROPBWUNIT_POS_X),  bn_defcdpi(DROPBWUNIT_POS_Y),
		bn_defcdpi(DROPBWUNIT_SIZE_X), bn_defcdpi(DROPBWUNIT_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);

	SetWindowPos(
		This->ui.tempTextHandle, NULL,
		bn_defcdpi(TEMP_POS_X),  bn_defcdpi(TEMP_POS_Y),
		bn_defcdpi(TEMP_SIZE_X), bn_defcdpi(TEMP_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);
	SetWindowPos(
		This->ui.tempUnitHandle, NULL,
		bn_defcdpi(DROPTUNIT_POS_X),  bn_defcdpi(DROPTUNIT_POS_Y),
		bn_defcdpi(DROPTUNIT_SIZE_X), bn_defcdpi(DROPTUNIT_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);

	SetWindowPos(
		This->ui.desiredNTextHandle, NULL,
		bn_defcdpi(DESIREDN_POS_X),  bn_defcdpi(DESIREDN_POS_Y),
		bn_defcdpi(DESIREDN_SIZE_X), bn_defcdpi(DESIREDN_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);
	SetWindowPos(
		This->ui.desiredNUnitHandle, NULL,
		bn_defcdpi(DROPDESIREDN_POS_X),  bn_defcdpi(DROPDESIREDN_POS_Y),
		bn_defcdpi(DROPDESIREDN_SIZE_X), bn_defcdpi(DROPDESIREDN_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);


	SetWindowPos(
		This->ui.inpTextHandle, NULL,
		bn_defcdpi(INP_POS_X),  bn_defcdpi(INP_POS_Y),
		bn_defcdpi(INP_SIZE_X), bn_defcdpi(INP_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);
	SetWindowPos(
		This->ui.inpUnitHandle, NULL,
		bn_defcdpi(DROPINP_POS_X),  bn_defcdpi(DROPINP_POS_Y),
		bn_defcdpi(DROPINP_SIZE_X), bn_defcdpi(DROPINP_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);

	SetWindowPos(
		This->ui.snrTextHandle, NULL,
		bn_defcdpi(SNR_POS_X),  bn_defcdpi(SNR_POS_Y),
		bn_defcdpi(SNR_SIZE_X), bn_defcdpi(SNR_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);
	SetWindowPos(
		This->ui.snrUnitHandle, NULL,
		bn_defcdpi(DROPSNR_POS_X),  bn_defcdpi(DROPSNR_POS_Y),
		bn_defcdpi(DROPSNR_SIZE_X), bn_defcdpi(DROPSNR_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);


	SetWindowPos(
		This->ui.resetBtn, NULL,
		bn_defcdpi(RESETBTN_POS_X),  bn_defcdpi(RESETBTN_POS_Y),
		bn_defcdpi(RESETBTN_SIZE_X), bn_defcdpi(RESETBTN_SIZE_Y),
		SWP_NOZORDER | SWP_NOOWNERZORDER
	);	
}
void bn_paint(bndata_t * restrict This, HDC hdc)
{
	// Draw image of circuit type
	HDC hdcmem = CreateCompatibleDC(hdc);
	HBITMAP oldbitmap = SelectObject(hdcmem, This->ui.bmp);

	BITMAP bitmap;
	GetObject(This->ui.bmp, sizeof bitmap, &bitmap);
	// Set StrechBlt mode
	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(
		hdc,
		0, 0,
		bn_defcdpi(BMP_SIZE_X), bn_defcdpi(BMP_SIZE_Y),
		hdcmem,
		0, 0,
		bitmap.bmWidth, bitmap.bmHeight,
		SRCCOPY
	);

	SelectObject(hdcmem, oldbitmap);
	DeleteDC(hdcmem);
	
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, This->normFont);

	RECT tr = {
		.left   = bn_defcdpi(DROPTYPE_S_POS_X),
		.top    = bn_defcdpi(DROPTYPE_S_POS_Y),
		.right  = tr.left + bn_defcdpi(DROPTYPE_S_SIZE_X),
		.bottom = tr.top  + bn_defcdpi(DROPTYPE_S_SIZE_Y)
	};
	DrawTextW(hdc, L"Circuit type:", -1, &tr, DT_SINGLELINE | DT_LEFT);

	tr.left   = bn_defcdpi(NOISE_S_POS_X);
	tr.top    = bn_defcdpi(NOISE_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(NOISE_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(NOISE_S_SIZE_Y);
	DrawTextW(hdc, L"Op-amp input noise voltage:", -1, &tr, DT_SINGLELINE | DT_CENTER);

	tr.left   = bn_defcdpi(BW_S_POS_X);
	tr.top    = bn_defcdpi(BW_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(BW_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(BW_S_SIZE_Y);
	DrawTextW(hdc, L"Bandwidth:", -1, &tr, DT_SINGLELINE | DT_CENTER);

	tr.left   = bn_defcdpi(TEMP_S_POS_X);
	tr.top    = bn_defcdpi(TEMP_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(TEMP_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(TEMP_S_SIZE_Y);
	DrawTextW(hdc, L"Operating temperature:", -1, &tr, DT_SINGLELINE | DT_CENTER);

	tr.left   = bn_defcdpi(DESIREDN_S_POS_X);
	tr.top    = bn_defcdpi(DESIREDN_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(DESIREDN_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(DESIREDN_S_SIZE_Y);
	DrawTextW(hdc, L"Desired output noise voltage:", -1, &tr, DT_SINGLELINE | DT_CENTER);

	
	tr.left   = bn_defcdpi(RESULT_S_POS_X);
	tr.top    = bn_defcdpi(RESULT_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(RESULT_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(RESULT_S_SIZE_Y);
	wchar_t result[MAX_RESULT];
	usize len = (usize)bn_printImpedance(result, MAX_RESULT, L"Optimal impedance based on noise: ", This->impedance);
	bn_printSNR(result + len, MAX_RESULT - len, L"\nSNR based on noise floor: ", This->snrFromNoise, This->ui.snrUnitIdx);
	DrawTextW(hdc, result, (int)wcsnlen_s(result, MAX_RESULT), &tr, DT_LEFT);


	tr.left   = bn_defcdpi(INP_S_POS_X);
	tr.top    = bn_defcdpi(INP_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(INP_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(INP_S_SIZE_Y);
	DrawTextW(hdc, L"Input level:", -1, &tr, DT_SINGLELINE | DT_CENTER);

	tr.left   = bn_defcdpi(SNR_S_POS_X);
	tr.top    = bn_defcdpi(SNR_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(SNR_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(SNR_S_SIZE_Y);
	DrawTextW(hdc, L"Desired signal-to-noise ratio (SNR):", -1, &tr, DT_SINGLELINE | DT_CENTER);


	tr.left   = bn_defcdpi(RESULTSNR_S_POS_X);
	tr.top    = bn_defcdpi(RESULTSNR_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(RESULTSNR_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(RESULTSNR_S_SIZE_Y);
	len = (usize)bn_printImpedance(result, MAX_RESULT, L"Optimal impedance based on SNR: ", This->snrImpedance);
	bn_printNoise(result + len, MAX_RESULT - len, L"\nNoise floor based on SNR: ", This->noiseFromSnr, This->ui.desiredNUnitIdx);
	DrawTextW(hdc, result, (int)wcsnlen_s(result, MAX_RESULT), &tr, DT_LEFT);

}
void bn_command(bndata_t * restrict This, WPARAM wp, LPARAM lp)
{
	// Text-box action
	if (HIWORD(wp) == EN_CHANGE)
	{
		// Get text value
		wchar txt[MAX_RESULT];
		txt[0] = L'\0';
		GetWindowTextW((HWND)lp, txt, MAX_RESULT);
		double value = _wtof(txt);

		switch (LOWORD(wp))
		{
		case IDT_NOISE:
			This->ui.noiseValue = value;
			break;
		case IDT_BANDWIDTH:
			This->ui.bwValue = value;
			break;
		case IDT_TEMP:
			This->ui.tempValue = value;
			break;
		case IDT_DESIREDN:
			This->ui.desiredNValue = value;
			break;
		case IDT_INP:
			This->ui.inpValue = value;
			break;
		case IDT_SNR:
			This->ui.snrValue = value;
			break;
		}

		bn_calculate(This);
	}
	// Drop-list action
	else if (HIWORD(wp) == CBN_SELCHANGE)
	{
		const usize idx = bn_getDropSel((HWND)lp);
		switch (LOWORD(wp))
		{
		case IDD_DROPTYPE:
			This->ui.circuitTypeIdx = idx;
			break;
		case IDD_DROPNOISEU:
			This->ui.noiseUnitIdx = idx;
			break;
		case IDD_DROPBWUNIT:
			This->ui.bwUnitIdx = idx;
			break;
		case IDD_DROPTUNIT:
			This->ui.tempUnitIdx = idx;
			break;
		case IDD_DROPDESIREDN:
			This->ui.desiredNUnitIdx = idx;
			break;
		case IDD_DROPINP:
			This->ui.inpUnitIdx = idx;
			break;
		case IDD_DROPSNR:
			This->ui.snrUnitIdx = idx;
			break;
		}

		bn_calculate(This);
	}
	// Normal button presses
	else
	{
		switch (LOWORD(wp))
		{
		// Reset button
		case IDM_RESET:
			bn_setDefaults(This);
			break;
		}
	}
}
void bn_calculate(bndata_t * restrict This)
{
	const double oldImpedance = This->impedance, oldSnrImpedance = This->snrImpedance,
		oldSnrFromNoise = This->snrFromNoise, oldNoiseFromSnr = This->noiseFromSnr;
	
	const int type = This->ui.circuitTypeIdx;

	const HBITMAP oldbmp = This->ui.bmp;
	This->ui.bmp = This->ui.bmps[type];

	double opNoise, temp, bw = 1.0, desNoise;
	
	if (bn_isnan(This->ui.noiseValue) || bn_isnan(This->ui.bwValue) ||
		bn_isnan(This->ui.tempValue)  || bn_isnan(This->ui.desiredNValue))
	{
		This->impedance = nan("");
	}
	else
	{
		// Updates UI elements based on mode
		switch (type)
		{
		case circtype_norm:
			break;
		case circtype_diff:
			break;
		case circtype_integrator:
			break;
		}
		
		bw = This->ui.bwValue;
		switch (This->ui.bwUnitIdx)
		{
		case bwutype_khz:
			bw *= 1e3;
			break;
		case bwutype_mhz:
			bw *= 1e6;
			break;
		case bwutype_ghz:
			bw *= 1e9;
			break;
		default:
			break;
		}

		opNoise  = bn_noiseVolts(This->ui.noiseValue,    bw, This->ui.noiseUnitIdx);
		desNoise = bn_noiseVolts(This->ui.desiredNValue, bw, This->ui.desiredNUnitIdx);


		temp = This->ui.tempValue;
		switch (This->ui.tempUnitIdx)
		{
		case tutype_celsius:
			temp += 273.15;
			break;
		case tutype_fahrenheit:
			temp = ((temp + 459.67) * 5.0) / 9.0;
			break;
		default:
			break;
		}

		// Calculates optimal impedance
		const double allowedNoise = desNoise - opNoise;

		if ((desNoise <= 0.0) || (opNoise <= 0.0) || (allowedNoise <= 0.0) || (temp < 0.0) || (bw <= 0.0))
		{
			This->impedance = nan("");
		}
		else
		{
			This->impedance = (allowedNoise * allowedNoise) / (4.0 * BOLTZMANN_CONSTANT * temp * bw);
		}
	}

	const bool calcSnrFromNoise = !(bn_isnan(This->impedance) || bn_isnan(This->ui.inpValue));
	const bool calcSnrImpedance = calcSnrFromNoise && !bn_isnan(This->ui.snrValue);

	if (!calcSnrImpedance)
	{
		This->snrImpedance = nan("");
	}

	if (calcSnrFromNoise)
	{
		double input = This->ui.inpValue;
		switch (This->ui.inpUnitIdx)
		{
		case inputype_uv:
			input /= 1e6;
			break;
		case inputype_mv:
			input /= 1e3;
			break;
		case inputype_volt:
			break;
		case inputype_dbuv:
			input -= 60.0;
			/* fallthrough */
		case inputype_dbmv:
			input -= 60.0;
			/* fallthough */
		case inputype_dbv:
			input = pow(10.0, input / 20.0);
			break;
		case inputype_dbu:
			input = pow(10.0, input / 20.0) * sqrt(0.6);
			break;
		}

		if (calcSnrImpedance)
		{
			double snr = This->ui.snrValue;
			switch (This->ui.snrUnitIdx)
			{
			case snrtype_db:
				snr = pow(10.0, snr / 20.0);
				break;
			default:
				break;
			}

			if ((input <= 0.0) || (snr <= 0.0))
			{
				This->snrImpedance = nan("");
			}
			else
			{
				This->noiseFromSnr = input / snr;
				const double allowedNoise = This->noiseFromSnr - opNoise;
				if (allowedNoise <= 0.0)
				{
					This->snrImpedance = nan("");
				}
				else
				{
					This->snrImpedance = (allowedNoise * allowedNoise) / (4.0 * BOLTZMANN_CONSTANT * temp * bw);
				}
			}
		}
		
		if (input <= 0.0)
		{
			This->snrFromNoise = nan("");
		}
		else
		{
			This->snrFromNoise = input / desNoise;
		}
	}
	else
	{
		This->snrFromNoise = nan("");
	}

	// Convert to correct units
	if (!bn_isnan(This->snrFromNoise))
	{
		if (This->ui.snrUnitIdx == snrtype_db)
		{
			This->snrFromNoise = 20.0 * log10(This->snrFromNoise);
		}
	}
	if (!bn_isnan(This->noiseFromSnr))
	{
		switch (This->ui.desiredNUnitIdx)
		{
		case nutype_nv_rthz:
			This->noiseFromSnr = (This->noiseFromSnr * 1e9) / sqrt(bw * NOISE_CONSTANT);
			break;
		case nutype_uv_rms:
			This->noiseFromSnr *= 1e6;
			break;
		case nutype_dbv:
			This->noiseFromSnr = 20.0 * log10(This->noiseFromSnr);
			break;
		default:
			break;
		}
	}
	
	// Refresh only if necessary
	if (This->ui.bmp != oldbmp)
	{
		const RECT br = {
			.left   = 0,
			.top    = 0,
			.right  = bn_defcdpi(BMP_SIZE_X),
			.bottom = bn_defcdpi(BMP_SIZE_Y)
		};
		InvalidateRect(This->hwnd, &br, FALSE);
	}
	if ((This->impedance != oldImpedance) || (This->snrFromNoise != oldSnrFromNoise))
	{
		const RECT tr = {
			.left   = bn_defcdpi(RESULT_S_POS_X),
			.top    = bn_defcdpi(RESULT_S_POS_Y),
			.right  = tr.left + bn_defcdpi(RESULT_S_SIZE_X),
			.bottom = tr.top  + bn_defcdpi(RESULT_S_SIZE_Y)
		};
		InvalidateRect(This->hwnd, &tr, TRUE);
	}
	if ((This->snrImpedance != oldSnrImpedance) || (This->noiseFromSnr != oldNoiseFromSnr))
	{
		const RECT tr = {
			.left   = bn_defcdpi(RESULTSNR_S_POS_X),
			.top    = bn_defcdpi(RESULTSNR_S_POS_Y),
			.right  = tr.left + bn_defcdpi(RESULTSNR_S_SIZE_X),
			.bottom = tr.top  + bn_defcdpi(RESULTSNR_S_SIZE_Y)
		};
		InvalidateRect(This->hwnd, &tr, TRUE);
	}
}
void bn_setDefaults(bndata_t * restrict This)
{
	SetWindowTextW(This->ui.noiseTextHandle,    DEF_NTEXT);
	SetWindowTextW(This->ui.bwTextHandle,       DEF_BWTEXT);
	SetWindowTextW(This->ui.tempTextHandle,     DEF_TTEXT);
	SetWindowTextW(This->ui.desiredNTextHandle, DEF_DNTEXT);

	SetWindowTextW(This->ui.inpTextHandle, DEF_INPTEXT);
	SetWindowTextW(This->ui.snrTextHandle, DEF_SNRTEXT);

	bn_setDropSel(This->ui.circuitTypeHandle,  This->ui.circuitTypeIdx  = circtype_default);
	bn_setDropSel(This->ui.noiseUnitHandle,    This->ui.noiseUnitIdx    = nutype_default);
	bn_setDropSel(This->ui.bwUnitHandle,       This->ui.bwUnitIdx       = bwutype_default);
	bn_setDropSel(This->ui.tempUnitHandle,     This->ui.tempUnitIdx     = tutype_default);
	bn_setDropSel(This->ui.desiredNUnitHandle, This->ui.desiredNUnitIdx = nutype_default);


	bn_setDropSel(This->ui.inpUnitHandle, This->ui.inpUnitIdx = inputype_default);
	bn_setDropSel(This->ui.snrUnitHandle, This->ui.snrUnitIdx = snrtype_default);

	SetFocus(This->ui.noiseTextHandle);
}
