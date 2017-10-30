#include "PlainText.h"

namespace Mile
{
   bool PlainText::Initialize( )
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

         return true;
      }

      stream.close( );
      return false;
   }

   std::string& PlainText::GetData( )
   {
      return m_data;
   }

   std::string PlainText::GetData( ) const
   {
      return m_data;
   }

   void PlainText::SetData( const std::string& newData )
   {
      m_data = newData;
   }

   bool PlainText::Save( )
   {
      std::ofstream stream( this->m_path );
      stream << m_data;
      stream.close( );
      return true;
   }
}