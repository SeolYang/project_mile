#pragma once
#include "Mile.h"
#include "string"

namespace Mile
{
	/**
	* 리터럴 구조체는 제공되는 문자타입에 따라 ANSI 또는 WIDE 문자열 리터럴을 선택하여 반환해줍니다.
	*/
	template <typename T>
	struct Literal
	{
		static const ANSICHAR Select( const ANSICHAR Ansi, const WIDECHAR )
		{
			return Ansi;
		}

		static const ANSICHAR* Select( const ANSICHAR* Ansi, const WIDECHAR* )
		{
			return Ansi;
		}
	};

	template <>
	struct Literal<WIDECHAR>
	{
		static const WIDECHAR Select( const ANSICHAR, const WIDECHAR Wide )
		{
			return Wide;
		}

		static const WIDECHAR* Select( const ANSICHAR*, const WIDECHAR* Wide )
		{
			return Wide;
		}
	};

#define LITERAL(CharType, Char) Literal<CharType>::Select(Char, L##Char)

	/**
	* 문자형의 래퍼(Wrapper)로 여러가지 추가 기능들을 제공하여준다.
	*/
	template <typename T>
	struct BaseChar
	{
		using CharType = T;
	public:
		/**
		* 문자를 대문자로 변환함.
		* @param Char 변환할 문자.
		*/
		static inline CharType ToUpper( CharType Char );
		/**
		* 문자를 소문자로 변환함.
		* @param Char 변환할 문자.
		*/
		static inline CharType ToLower( CharType Char );
		/**
		* 문자가 대문자인지 검사함.
		* @param Char 검사할 문자.
		*/
		static inline bool IsUpper( CharType Char );
		/**
		* 문자가 소문자인지 검사함.
		* @param Char 검사할 문자.
		*/
		static inline bool IsLower( CharType Char );
		/**
		* 문자가 알파벳인지 검사함.
		* @param Char 검사할 문자.
		*/
		static inline bool IsAlpha( CharType Char );
		/**
		* 문자가 구두점인지 검사함.
		* @param Char 검사할 문자.
		*/
		static inline bool IsPunct( CharType Char );
		/**
		* 문자가 알파벳 또는 숫자인지 검사함.
		* @param Char 검사할 문자.
		*/
		static inline bool IsAlnum( CharType Char );
		/**
		* 문자가 10진수인지 검사함.
		* @param Char 검사할 문자.
		*/
		static inline bool IsDigit( CharType Char );

		/**
		* 문자가 밑줄인지 검사함.
		* @param Char 검사할 문자.
		*/
		static inline bool IsUnderscore( CharType Char )
		{
			return Char == LITERAL( CharType, '_' );
		}
	};

	template <>
	inline BaseChar<WIDECHAR>::CharType BaseChar<WIDECHAR>::ToUpper( CharType Char )
	{
		towupper( Char );
	}

	template <>
	inline BaseChar<WIDECHAR>::CharType BaseChar<WIDECHAR>::ToLower( CharType Char )
	{
		towlower( Char );
	}

	template <>
	inline bool BaseChar<WIDECHAR>::IsUpper( CharType Char )
	{
		return iswupper( Char );
	}

	template <>
	inline bool BaseChar<WIDECHAR>::IsLower( CharType Char )
	{
		return iswlower( Char );
	}

	template <>
	inline bool BaseChar<WIDECHAR>::IsAlpha( CharType Char )
	{
		return iswalpha( Char );
	}

	template <>
	inline bool BaseChar<WIDECHAR>::IsPunct( CharType Char )
	{
		return iswpunct( Char );
	}

	template <>
	inline bool BaseChar<WIDECHAR>::IsAlnum( CharType Char )
	{
		return iswalnum( Char );
	}

	template <>
	inline bool BaseChar<WIDECHAR>::IsDigit( CharType Char )
	{
		return iswdigit( Char );
	}

	template <>
	inline BaseChar<ANSICHAR>::CharType BaseChar<ANSICHAR>::ToUpper( CharType Char )
	{
		toupper( Char );
	}

	template <>
	inline BaseChar<ANSICHAR>::CharType BaseChar<ANSICHAR>::ToLower( CharType Char )
	{
		tolower( Char );
	}

	template <>
	inline bool BaseChar<ANSICHAR>::IsUpper( CharType Char )
	{
		return isupper( Char );
	}

	template <>
	inline bool BaseChar<ANSICHAR>::IsLower( CharType Char )
	{
		return islower( Char );
	}

	template <>
	inline bool BaseChar<ANSICHAR>::IsAlpha( CharType Char )
	{
		return isalpha( Char );
	}

	template <>
	inline bool BaseChar<ANSICHAR>::IsPunct( CharType Char )
	{
		return ispunct( Char );
	}

	template <>
	inline bool BaseChar<ANSICHAR>::IsAlnum( CharType Char )
	{
		return isalnum( Char );
	}

	template <>
	inline bool BaseChar<ANSICHAR>::IsDigit( CharType Char )
	{
		return isdigit( Char );
	}

	/** Mile Engine(API)의 기본 문자타입 */
	using MChar = BaseChar<MCHAR>;
}