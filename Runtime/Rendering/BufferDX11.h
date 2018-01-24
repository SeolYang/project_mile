#pragma once
#include "ResourceDX11.h"
   
namespace Mile
{
   class MEAPI BufferDX11 : public ResourceDX11
   {
   public:
      BufferDX11( RendererDX11* renderer ) : 
         m_bIsMapped( false ),
         m_buffer( nullptr ),
         ResourceDX11( renderer )
      {
      }

      ~BufferDX11( )
      {
         SafeRelease( m_buffer );
      }

      virtual ID3D11Resource* GetResource( ) override;
      D3D11_BUFFER_DESC GetDesc( ) const { return m_desc; }

      virtual void* Map( ID3D11DeviceContext& deviceContext ) { return nullptr;  }
      virtual bool UnMap( ID3D11DeviceContext& deviceContext ) { return false; }
      virtual bool UnMapImmediately( ) final;

   protected:
      ID3D11Buffer*     m_buffer;
      D3D11_BUFFER_DESC m_desc;
      bool              m_bIsMapped;

   };
}