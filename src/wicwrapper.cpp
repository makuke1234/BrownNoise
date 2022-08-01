#include "wicwrapper.h"

extern "C"
{

static struct coautoinit
{
	coautoinit() noexcept
	{
		CoInitialize(NULL);
	}
	~coautoinit() noexcept
	{
		CoUninitialize();
	}

} s_coautoinitinstance;

static IStream * wic_CreateStreamOnResource(HINSTANCE hInst, LPCWSTR lpName, LPCWSTR lpType)
{
    // initialize return value

    IStream * ipStream = NULL;
 
    // find the resource

    HRSRC hrsrc = FindResourceW(hInst, lpName, lpType);
    if (hrsrc == NULL)
    {
		return NULL;
	}
 
    // load the resource

    DWORD dwResourceSize = SizeofResource(hInst, hrsrc);
    HGLOBAL hglbImage = LoadResource(hInst, hrsrc);
    if (hglbImage == NULL)
    {
		return NULL;
	}
 
    // lock the resource, getting a pointer to its data

    LPVOID pvSourceResourceData = LockResource(hglbImage);
    if (pvSourceResourceData == NULL)
    {
		return NULL;
	}
 
    // allocate memory to hold the resource data

    HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
    if (hgblResourceData == NULL)
	{
		return NULL;
	}
 
    // get a pointer to the allocated memory

    LPVOID pvResourceData = GlobalLock(hgblResourceData);
    if (pvResourceData == NULL)
	{
        GlobalFree(hgblResourceData);
		return NULL;
	}
    // copy the data from the resource to the new memory block

    CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
    GlobalUnlock(hgblResourceData);
 
    // create a stream on the HGLOBAL containing the data

    if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
	{
		return ipStream;
	}
 
    // couldn't create stream; free the memory

    GlobalFree(hgblResourceData);
 
    // no need to unlock or free the resource

    return ipStream;
}

// Loads a PNG image from the specified stream (using Windows Imaging Component).

static IWICBitmapSource * wic_LoadBitmapFromStream(IStream * ipImageStream)
{
    // initialize return value

    IWICBitmapSource * ipBitmap = NULL;
 
    // load WIC's PNG decoder

    IWICBitmapDecoder * ipDecoder = NULL;
    if (FAILED(CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder))))
	{
		ipDecoder->Release();
		return NULL;
	}
 
    // load the PNG

    if (FAILED(ipDecoder->Initialize(ipImageStream, WICDecodeMetadataCacheOnLoad)))
    {
		ipDecoder->Release();
		return NULL;
	}
    // check for the presence of the first frame in the bitmap

    UINT nFrameCount = 0;
    if (FAILED(ipDecoder->GetFrameCount(&nFrameCount)) || nFrameCount != 1)
	{
		ipDecoder->Release();
		return NULL;
	}
 
    // load the first frame (i.e., the image)

    IWICBitmapFrameDecode * ipFrame = NULL;
    if (FAILED(ipDecoder->GetFrame(0, &ipFrame)))
	{
		ipDecoder->Release();
		return NULL;
	}
 
    // convert the image to 32bpp BGRA format with pre-multiplied alpha

    //   (it may not be stored in that format natively in the PNG resource,

    //   but we need this format to create the DIB to use on-screen)

    WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
    ipFrame->Release();
    ipDecoder->Release();
 
	return ipBitmap;
}

// 32-bit DIB from the specified WIC bitmap.
static HBITMAP wic_CreateHBITMAPfromIWIC(IWICBitmapSource * ipBitmap)
{
    // initialize return value

    HBITMAP hbmp = NULL;
 
    // get image attributes and check for valid image

    UINT width = 0;
    UINT height = 0;
    if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0)
    {
		return NULL;
	}
 
    // prepare structure giving bitmap information (negative height indicates a top-down DIB)

    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = width;
    bminfo.bmiHeader.biHeight = -((LONG) height);
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;
 
    // create a DIB section that can hold the image

    void * pvImageBits = NULL;
    HDC hdcScreen = GetDC(NULL);
    hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);
    if (hbmp == NULL)
	{
		return NULL;
	}
 
    // extract the image into the HBITMAP

    const UINT cbStride = width * 4;
    const UINT cbImage = cbStride * height;
    if (FAILED(ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE *>(pvImageBits))))
    {
        // couldn't extract image; delete HBITMAP

        DeleteObject(hbmp);
        hbmp = NULL;
    }
 
    return hbmp;
}

HBITMAP wic_LoadPNG(HINSTANCE hinst, LPCWSTR rscName)
{
	HBITMAP hbmpSplash = NULL;
 
    // load the PNG image data into a stream

    IStream * ipImageStream = wic_CreateStreamOnResource(hinst, rscName, L"PNG");
    if (ipImageStream == NULL)
	{
		return NULL;
	}

    // load the bitmap with WIC

    IWICBitmapSource * ipBitmap = wic_LoadBitmapFromStream(ipImageStream);
    if (ipBitmap == NULL)
	{
		ipImageStream->Release();
		return NULL;
	}
    // create a HBITMAP containing the image

    hbmpSplash = wic_CreateHBITMAPfromIWIC(ipBitmap);
    ipBitmap->Release();
    ipImageStream->Release();

    return hbmpSplash;
}

}
