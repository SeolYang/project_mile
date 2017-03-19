#pragma once
#include "Mile.h"

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
        BaseChar( )
        {
        }

        BaseChar( CharType Char ) :
            Character( Char )
        {
        }

        BaseChar& operator=( const BaseChar& Value )
        {
            Character = Value.Character;
            return ( *this );
        }

        BaseChar& operator=( CharType Value )
        {
            Character = Value;
            return ( *this );
        }

        bool operator==( CharType Value ) const
        {
            return ( Character == Value );
        }

        bool operator!=( CharType Value ) const
        {
            return ( Character != Value );
        }

        bool operator==( const BaseChar& Value ) const
        {
            return ( Character == Value.Character );
        }

        bool operator!=( const BaseChar& Value ) const
        {
            return ( Character == Value.Character );
        }

        operator CharType( ) const
        {
            return Character;
        }

        /**
        * 문자를 대문자로 변환함.
        * @param Char 변환할 문자.
        */
        inline CharType ToUpper( );
        /**
        * 문자를 소문자로 변환함.
        * @param Char 변환할 문자.
        */
        inline CharType ToLower( );
        /**
        * 문자가 대문자인지 검사함.
        * @param Char 검사할 문자.
        */
        inline bool IsUpper( );
        /**
        * 문자가 소문자인지 검사함.
        * @param Char 검사할 문자.
        */
        inline bool IsLower( );
        /**
        * 문자가 알파벳인지 검사함.
        * @param Char 검사할 문자.
        */
        inline bool IsAlpha( );
        /**
        * 문자가 구두점인지 검사함.
        * @param Char 검사할 문자.
        */
        inline bool IsPunct( );
        /**
        * 문자가 알파벳 또는 숫자인지 검사함.
        * @param Char 검사할 문자.
        */
        inline bool IsAlnum( );
        /**
        * 문자가 10진수인지 검사함.
        * @param Char 검사할 문자.
        */
        inline bool IsDigit( );

        /**
        * 문자가 밑줄인지 검사함.
        * @param Char 검사할 문자.
        */
        inline bool IsUnderscore( )
        {
            return Character == LITERAL( CharType, '_' );
        }

    public:
        CharType Character;

    };

    template <>
    inline BaseChar<WIDECHAR>::CharType BaseChar<WIDECHAR>::ToUpper( )
    {
        towupper( Character );
    }

    template <>
    inline BaseChar<WIDECHAR>::CharType BaseChar<WIDECHAR>::ToLower( )
    {
        towlower( Character );
    }

    template <>
    inline bool BaseChar<WIDECHAR>::IsUpper( )
    {
        return static_cast< bool >( iswupper( Character ) );
    }

    template <>
    inline bool BaseChar<WIDECHAR>::IsLower( )
    {
        return static_cast< bool >( iswlower( Character ) );
    }

    template <>
    inline bool BaseChar<WIDECHAR>::IsAlpha( )
    {
        return static_cast< bool >( iswalpha( Character ) );
    }

    template <>
    inline bool BaseChar<WIDECHAR>::IsPunct( )
    {
        return static_cast< bool >( iswpunct( Character ) );
    }

    template <>
    inline bool BaseChar<WIDECHAR>::IsAlnum( )
    {
        return static_cast< bool >( iswalnum( Character ) );
    }

    template <>
    inline bool BaseChar<WIDECHAR>::IsDigit( )
    {
        return static_cast< bool >( iswdigit( Character ) );
    }

    template <>
    inline BaseChar<ANSICHAR>::CharType BaseChar<ANSICHAR>::ToUpper( )
    {
        toupper( Character );
    }

    template <>
    inline BaseChar<ANSICHAR>::CharType BaseChar<ANSICHAR>::ToLower( )
    {
        tolower( Character );
    }

    template <>
    inline bool BaseChar<ANSICHAR>::IsUpper( )
    {
        return static_cast< bool >( isupper( Character ) );
    }

    template <>
    inline bool BaseChar<ANSICHAR>::IsLower( )
    {
        return static_cast< bool >( islower( Character ) );
    }

    template <>
    inline bool BaseChar<ANSICHAR>::IsAlpha( )
    {
        return static_cast< bool >( isalpha( Character ) );
    }

    template <>
    inline bool BaseChar<ANSICHAR>::IsPunct( )
    {
        return static_cast< bool >( ispunct( Character ) );
    }

    template <>
    inline bool BaseChar<ANSICHAR>::IsAlnum( )
    {
        return static_cast< bool >( isalnum( Character ) );
    }

    template <>
    inline bool BaseChar<ANSICHAR>::IsDigit( )
    {
        return static_cast< bool >( isdigit( Character ) );
    }

    /** Mile Engine(API)의 기본 문자타입 */
    using MChar = BaseChar<WIDECHAR>;
}