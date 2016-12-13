#pragma once
#pragma warning(disable : 4251)
#pragma warning(disable : 4512)
#pragma warning(disable : 4996)

#include <iostream>
#include <string>

#undef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include <memory>

/** Data structures */
#include <vector>
#include <stack>
#include <queue>

#define MILE_LIBRARYEXPORT __declspec(dllexport)
#define MILE_LIBRARYIMPORT __declspec(dllimport)

#ifdef MILE_BUILD
#define MILE_API          MILE_LIBRARYEXPORT
#else
#define MILE_API          MILE_LIBRARYIMPORT
#endif

#define MILE_FORCEINLINE  __forceinline

#define MS_ALIGN(N)       __declspec(align(N))
#define ALGIN_4			  MS_ALIGN(4)
#define ALGIN_8			  MS_ALIGN(8)
#define ALGIN_16          MS_ALIGN(16)
#define ALGIN_32          MS_ALIGN(32)
#define ALGIN_64          MS_ALIGN(64)
#define ALGIN_128         MS_ALIGN(128)

#ifndef ASSERT
#ifdef _DEBUG
#include <cassert>
#define ASSERT(Condition) { if( !(Condition) ) { __debugbreak( ); } }
#define ASSERT_MSG(Condition, Message) \
	do  \
	{ \
		if( !(Condition) ) \
		{					\
			std::cerr << "#Assertion " #Condition " failed in " << __FILE__ \
					  << " line " << __LINE__ << ": " << Message << std::endl; \
			ASSERT( Condition );\
		}					\
	} while(false)
#else
#define ASSERT(Condition) ((void)0)
#define ASSERT_MSG(Condition, Message)
#endif
#endif

#define UNUSED_PARAM(Param) ((void)Param)

namespace Mile
{
    /** 부호없는 기본 타입 */
    using uint8 = unsigned char;
    using uint16 = unsigned short int;
    using uint32 = unsigned int;
    using uint64 = unsigned long long;

    /** 부호있는 기본 타입 */
    using int8 = signed char;
    using int16 = signed short int;
    using int32 = signed int;
    using int64 = signed long long;

    /** 문자 타입 */
    using ANSICHAR = char;
    using WIDECHAR = wchar_t;

    template<typename Ty>
    class TypeDisplayer;
}