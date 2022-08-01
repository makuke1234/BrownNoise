#ifndef WICWRAPPER_H
#define WICWRAPPER_H

#include "winapi.h"

#include <ObjIdl.h>
#include <wincodec.h>

#ifdef __cplusplus
extern "C"
{
#endif

HBITMAP wic_LoadPNG(HINSTANCE hinst, LPCWSTR rscName);


#ifdef __cplusplus
}
#endif

#endif
