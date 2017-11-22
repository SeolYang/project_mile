#pragma once
#pragma warning( disable : 4251 )

#include <vector>
#include <array>

#include <memory>
#include <functional>

#include <locale>
#include <codecvt>
#include <string>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

#define MEAPI __declspec(dllexport)

#include "json.hpp"
using json = nlohmann::json;

namespace Mile
{
   using String = std::wstring;
   using Float = float;

   static std::string BoolSerialize( bool value )
   {
      if ( value )
      {
         return "true";
      }
      
      return "false";
   }

   /**
   * @brief    주어진 문자열을 토큰을 기준으로 나눕니다.
   * @param    Array를 만드는데 사용될 할당자
   * @param    나눌 문자열
   * @param    토큰
   * @return   나누어진 문자열들의 배열
   */
   static std::vector<String> SplitStr( const String& str, char token )
   {
      std::vector<String> tempArr{ };
      String tempStr{ };

      size_t length = str.length( );

      for ( size_t idx = 0; idx < length; ++idx )
      {
         if ( str[ idx ] != token )
         {
            tempStr += str[ idx ];
         }

         if ( str[ idx ] == token || idx == ( length - 1 ) )
         {
            tempArr.push_back( tempStr );
            tempStr.clear( );
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
   static String CombineStr( std::vector<String> strings, const String& token = TEXT("") )
   {
      String temp{ };

      for ( auto str : strings )
      {
         temp += str;
         temp += token;
      }

      return temp;
   }

   /**
   * @brief  Release를 통해 메모리 할당 해제를 하는 인스턴스를 안전하게 해제시켜줍니다.
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

   /**
   * @brief Delete를 통해 메모리를 할당 해제 해야하는 인스턴스를 안전하게 해제시켜줍니다.
   */
   template <typename T>
   void SafeDelete( T& target )
   {
      if ( target != nullptr )
      {
         delete target;
         target = nullptr;
      }
   }

   template <typename T>
   void SafeFree( T& target )
   {
      if ( target != nullptr )
      {
         std::free( target );
         target = nullptr;
      }
   }

   static String String2WString( const std::string& str )
   {
      if ( str.empty( ) )
      {
         return TEXT( "" );
      }
      std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
      return converter.from_bytes( str );
   }

   static std::string WString2String( const String& str )
   {
      if ( str.empty( ) )
      {
         return std::string( );
      }

      std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
      return converter.to_bytes( str );
   }
}