#pragma once
#include "ResourceDX11.h"
   
namespace Mile
{
   class MEAPI BufferDX11 : public ResourceDX11
   {
   public:
      BufferDX11( ) : m_buffer( nullptr )
      {
      }

      ~BufferDX11( )
      {
         SafeRelease( m_buffer );
      }

      virtual ID3D11Resource* GetResource( ) override;

      virtual void* Map( ) { return nullptr;  }
      virtual void UnMap( ) { }

   private:
      ID3D11Buffer*     m_buffer;

   };
}