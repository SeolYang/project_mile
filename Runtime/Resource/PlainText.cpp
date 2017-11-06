#include "PlainText.h"

namespace Mile
{
   bool PlainText::Init( )
   {
      m_data = TEXT("");

      std::wifstream stream( this->m_path );
      if ( stream.is_open( ) )
      {
         String tmp;
         while ( std::getline( stream, tmp ) )
         {
            m_data += tmp;
            m_data += '\n';
         }

         return true;
      }

      stream.close( );
      return false;
   }

   String& PlainText::GetData( )
   {
      return m_data;
   }

   String PlainText::GetData( ) const
   {
      return m_data;
   }

   void PlainText::SetData( const String& newData )
   {
      m_data = newData;
   }

   bool PlainText::Save( )
   {
      std::wofstream stream( this->m_path );
      stream << m_data;
      stream.close( );
      return true;
   }
}