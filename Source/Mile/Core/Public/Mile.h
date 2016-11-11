#pragma once
#pragma warning(disable : 4251)
#pragma warning(disable : 4512)
#pragma warning(disable : 4996)

#define MILE_LIBRARYEXPORT __declspec(dllexport)
#define MILE_LIBRARYIMPORT __declspec(dllimport)

#ifdef MILE_BUILD
#define MILE_API          MILE_LIBRARYEXPORT
#else
#define MILE_API          MILE_LIBRARYIMPORT
#endif

#define MILE_FORCEINLINE  __forceinline

#define MS_ALIGN(N)       __declspec(align(N))
#define ALGIN_16          MS_ALIGN(16)

/** 부호없는 기본 타입 */
using uint8     = unsigned char;
using uint16    = unsigned short int;
using uint32    = unsigned int;
using uint64    = unsigned long long;

/** 부호있는 기본 타입 */
using int8      = signed char;
using int16     = signed short int;
using int32     = signed int;
using int64     = signed long long;

/** 문자 타입 */
using ANSICHAR = char;
using WIDECHAR = wchar_t;
using MCHAR = WIDECHAR;

#ifndef ASSERT
    #ifdef _DEBUG
    #include <cassert>
    #define ASSERT(x) { assert(x) }
    #else
    #define ASSERT(x)
    #endif
#endif