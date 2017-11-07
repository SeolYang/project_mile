#pragma once
#include "Resource.h"

namespace Mile
{
   class MEAPI PlainText : public Resource
   {
   public:
      PlainText( Context* context, const String& path ) 
         : Resource( context,
                     path, 
                     ResourceType::RT_PlainText )
      {
      }

      virtual bool Init( ) override;
      virtual bool Save( const String& filePath ) override;
      virtual bool Save( ) override;

      String& GetData( );
      String GetData( ) const;

      void SetData( const String& newData );


   private:
      String m_data;

   };
}