#ifndef WINAPI_H
#define WINAPI_H

#ifndef NOMINMAX
	#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_EXTRA_LEAN
	#define WIN32_EXTRA_LEAN
#endif

#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <dwmapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

typedef ssize_t isize;
typedef size_t usize;

typedef unsigned char uchar;
typedef wchar_t wchar;

typedef void * vptr;
typedef const void * cvptr;
typedef intptr_t iptr;

typedef const char * cstr;
typedef const wchar * cwstr;

typedef float f32;
typedef double f64;
typedef long double f128;



#endif
