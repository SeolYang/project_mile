#pragma once

#pragma warning( disable : 4251 )

#include "Rumia\Allocator.h"
#include "Rumia\Array.h"

#define MEAPI __declspec(dllexport)

namespace Mile
{
    using namespace Rumia;

    /**
    * @brief    주어진 문자열을 토큰을 기준으로 나눕니다.
    * @param    Array를 만드는데 사용될 할당자
    * @param    나눌 문자열
    * @param    토큰
    * @return   나누어진 문자열들의 배열
    */
    static Array<std::string> SplitStr( Allocator& allocator, const std::string& str, char token )
    {
        Array<std::string> tempArr{ allocator };
        size_t before = 0;
        for ( size_t idx = 0; idx < str.length( ); ++idx )
        {
            if ( str[ idx ] == token )
            {
                std::string tempStr = str.substr( before, idx - before );
                if ( !tempStr.empty( ) )
                {
                    tempArr.PushBack( std::move( tempStr ) );
                }
                before = idx;
            }
        }

        return tempArr;
    }

    /**
    * @brief    주어진 문자열들의 배열을 합칩니다.
    * @param    합칠 문자열들의 배열
    * @param    문자열들을 합치면서 중간에 들어갈 토큰
    * @return   합쳐진 문자열
    */
    static std::string CombineStr( Array<std::string> strings, const std::string& token = "" )
    {
        std::string temp{ };

        for ( auto str : strings )
        {
            temp += str;
            temp += token;
        }

        return temp;
    }

    /**
    @brief  Release를 통해 메모리 할당 해제를 하는 인스턴스를 안전하게 해제시켜줍니다.
    */
    template <typename T>
    void SafeRelease( T& target )
    {
        if ( target != nullptr )
        {
            target->Release( );
            target = nullptr;
        }
    }
}