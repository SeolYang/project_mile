#include "PlainText.h"

namespace Mile
{
   template<>
   bool PlainText<String>::Init( )
   {
      m_data = TEXT( "" );

      std::wifstream stream( this->m_path );
      if ( stream.is_open( ) )
      {
         String tmp;
         while ( std::getline( stream, tmp ) )
         {
            m_data += tmp;
            m_data += '\n';
         }

         stream.close( );
         return true;
      }

      stream.close( );
      return false;
   }

   template<>
   bool PlainText<std::string>::Init( )
   {
      m_data = "";

      std::ifstream stream( this->m_path );
      if ( stream.is_open( ) )
      {
         std::string tmp;
         while ( std::getline( stream, tmp ) )
         {
            m_data += tmp;
            m_data += '\n';
         }

         stream.close( );
         return true;
      }

      stream.close( );
      return false;
   }

   template <>
   bool PlainText<String>::Save( const String& filePath )
   {
      std::wofstream stream( filePath );
      stream << m_data;
      stream.close( );
      return true;
   }

   template <>
   bool PlainText<std::string>::Save( const String& filePath )
   {
      std::ofstream stream( filePath );
      stream << m_data;
      stream.close( );
      return true;
   }
}