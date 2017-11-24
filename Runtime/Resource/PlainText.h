#pragma once
#include "Resource.h"

namespace Mile
{
   template <typename StrType = String>
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
      virtual bool SaveTo( const String& filePath ) override;

      StrType GetData( ) const { return m_data; }

      void SetData( const StrType& newData )
      {
         m_data = newData;
      }

   private:
      StrType m_data;

   };
}