#include "app.h"
#include "resource.h"
#include "error.h"
#include "winapi.h"


static struct
{
	const wchar * droptype[DROPTYPE_SIZE];
	const wchar * dropnoiseu[DROPNOISEU_SIZE];
	const wchar * dropbwunit[DROPBWUNIT_SIZE];

} s_def = {
	.droptype = {
		L"Resistor feedback",
		L"Differentiator",
		L"Integrator"
	},
	.dropnoiseu = {
		L"nV/\u221AHz",
		L"\u00B5V RMS"
	},
	.dropbwunit = {
		L"Hz",
		L"KHz",
		L"MHz",
		L"GHz"
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
			.circuitTypeIdx    = circtype_norm,
			
			.noiseTextHandle = NULL,
			.noiseUnitHandle = NULL,
			.noiseUnitIdx    = nutype_nv_rthz,
			.noiseValue      = 0.0,

			.bwTextHandle = NULL,
			.bwUnitHandle = NULL,
			.bwUnitIdx    = bwutype_khz,
			.bwValue      = 0.0,
			
			.desiredNTextHandle = NULL,
			.desiredNUnitHandle = NULL,
			.desiredNUnitIdx    = nutype_nv_rthz,
			.desiredNValue      = 0.0
		},

		.impedance = 0.0
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
	const wchar ** restrict options, usize numopt, usize defopt
)
{
	assert(parent != NULL);
	assert(options != NULL);
	assert(numopt > 0);
	assert(((WPARAM)numopt) != (WPARAM)-1);
	assert(options[0] != NULL);
	assert(defopt < numopt);

	HWND box = CreateWindowExW(
		0,
		L"combobox", NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_DROPDOWNLIST,
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

	// Set default value
	const WPARAM wpdefopt = (WPARAM)defopt;
	assert(wpdefopt != (WPARAM)-1);
	SendMessageW(box, CB_SETCURSEL, wpdefopt, 0);

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
		WS_CHILD | WS_VISIBLE | ES_LEFT,
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
void bn_setFont(HWND hwnd, HFONT hfont)
{
	SendMessageW(hwnd, WM_SETFONT, (WPARAM)hfont, FALSE);
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

	This->ui.bmps[circtype_norm]       = LoadBitmapW(This->hinst, MAKEINTRESOURCEW(IDB_NORM));
	This->ui.bmps[circtype_diff]       = LoadBitmapW(This->hinst, MAKEINTRESOURCEW(IDB_DIFF));
	This->ui.bmps[circtype_integrator] = LoadBitmapW(This->hinst, MAKEINTRESOURCEW(IDB_INT));

	assert(This->ui.bmps[circtype_norm]       != NULL);
	assert(This->ui.bmps[circtype_diff]       != NULL);
	assert(This->ui.bmps[circtype_integrator] != NULL);

	This->ui.bmp = This->ui.bmps[This->ui.circuitTypeIdx];

	This->ui.circuitTypeHandle = bn_createDrop(
		bn_defcdpi(DROPTYPE_POS_X), bn_defcdpi(DROPTYPE_POS_Y),
		bn_defcdpi(DROPTYPE_SIZE_X), bn_defcdpi(DROPTYPE_SIZE_Y),
		This->hwnd, (HMENU)IDD_DROPTYPE,
		s_def.droptype, DROPTYPE_SIZE, This->ui.circuitTypeIdx
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
		s_def.dropnoiseu, DROPNOISEU_SIZE, This->ui.noiseUnitIdx
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
		s_def.dropbwunit, DROPBWUNIT_SIZE, This->ui.bwUnitIdx
	);
	bn_setFont(This->ui.bwUnitHandle, This->normFont);

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
		s_def.dropnoiseu, DROPNOISEU_SIZE, This->ui.desiredNUnitIdx
	);
	bn_setFont(This->ui.desiredNUnitHandle, This->normFont);

	This->ui.resetBtn = CreateWindowExW(
		0,
		L"button",
		L"Reset",
		WS_CHILD | WS_VISIBLE,
		bn_defcdpi(RESETBTN_POS_X),  bn_defcdpi(RESETBTN_POS_Y),
		bn_defcdpi(RESETBTN_SIZE_X), bn_defcdpi(RESETBTN_SIZE_Y),
		This->hwnd, (HMENU)IDM_RESET,
		This->hinst, NULL
	);
	bn_setFont(This->ui.resetBtn, This->normFont);
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
	DrawTextW(hdc, L"Select circuit type:", -1, &tr, DT_SINGLELINE | DT_LEFT);

	tr.left   = bn_defcdpi(NOISE_S_POS_X);
	tr.top    = bn_defcdpi(NOISE_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(NOISE_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(NOISE_S_SIZE_Y);
	DrawTextW(hdc, L"Op-amp input noise voltage:", -1, &tr, DT_SINGLELINE | DT_LEFT);

	tr.left   = bn_defcdpi(BW_S_POS_X);
	tr.top    = bn_defcdpi(BW_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(BW_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(BW_S_SIZE_Y);
	DrawTextW(hdc, L"Bandwidth:", -1, &tr, DT_SINGLELINE | DT_LEFT);

	tr.left   = bn_defcdpi(DESIREDN_S_POS_X);
	tr.top    = bn_defcdpi(DESIREDN_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(DESIREDN_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(DESIREDN_S_SIZE_Y);
	DrawTextW(hdc, L"Desired noise voltage:", -1, &tr, DT_SINGLELINE | DT_LEFT);

	
	tr.left   = bn_defcdpi(RESULT_S_POS_X);
	tr.top    = bn_defcdpi(RESULT_S_POS_Y);
	tr.right  = tr.left + bn_defcdpi(RESULT_S_SIZE_X);
	tr.bottom = tr.top  + bn_defcdpi(RESULT_S_SIZE_Y);

	wchar_t result[MAX_RESULT];
	result[0] = L'\0';
	swprintf_s(result, MAX_RESULT - 1, L"Optimal impedance: %.3g KOhms", This->impedance / 1000.0);

	DrawTextW(hdc, result, (int)wcsnlen_s(result, MAX_RESULT), &tr, DT_SINGLELINE | DT_LEFT);
}
void bn_command(bndata_t * restrict This, WPARAM wp, LPARAM lp)
{
	// Text-box action
	if (HIWORD(wp) == EN_CHANGE)
	{
		// Get text value
		double value = 0.0;

		switch (LOWORD(wp))
		{
		case IDT_NOISE:
			// Get value
			break;
		case IDT_BANDWIDTH:
			break;
		case IDT_DESIREDN:
			break;
		}

		bn_update(This);
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
		case IDD_DROPDESIREDN:
			This->ui.desiredNUnitIdx = idx;
			break;
		}

		bn_update(This);
	}
	// Normal button presses
	else
	{
		switch (LOWORD(wp))
		{
		// Reset button
		case IDM_RESET:
			
			break;
		}
	}
}
void bn_update(bndata_t * restrict This)
{
	const double oldImpedance = This->impedance;
	
	// Updates UI elements based on mode
	const int type = This->ui.circuitTypeIdx;

	const HBITMAP oldbmp = This->ui.bmp;
	This->ui.bmp = This->ui.bmps[type];
	switch (type)
	{
	case circtype_norm:
		break;
	case circtype_diff:
		break;
	case circtype_integrator:
		break;
	}
	
	// Calculate optimal impedance
	
	// Refresh only if necessary
	if ((This->ui.bmp != oldbmp) || (This->impedance != oldImpedance))
	{
		InvalidateRect(This->hwnd, NULL, FALSE);
	}
}
