#pragma once
#include "Resource.h"

namespace Mile
{
   class MEAPI PlainText : public Resource
   {
   public:
      PlainText( const String& path ) 
         : Resource( path, 
                     ResourceType::RT_PlainText )
      {
      }

      virtual bool Initialize( ) override;

      String& GetData( );
      String GetData( ) const;

      void SetData( const String& newData );

      virtual bool Save( ) override;

   private:
      String m_data;

   };
}