#pragma once
#include "Resource.h"

namespace Mile
{
   class MEAPI PlainText : public Resource
   {
   public:
      PlainText( const std::string& path ) 
         : Resource( path, 
                     ResourceType::RT_PlainText )
      {
      }

      virtual bool Initialize( ) override;

      std::string& GetData( );
      std::string GetData( ) const;

      void SetData( const std::string& newData );

      virtual bool Save( ) override;

   private:
      std::string m_data;

   };
}