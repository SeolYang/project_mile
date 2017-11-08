#pragma once

#include "ResourceDX11.h"

namespace Mile
{
   class MEAPI DepthStencilBufferDX11 : public ResourceDX11
   {
   public:
      DepthStencilBufferDX11( RendererDX11* renderer ) :
         m_bStencilEnabled( true ),
         ResourceDX11( renderer )
      {

      }

      ~DepthStencilBufferDX11( )
      {
         SafeRelease( m_depthStencilView );
         SafeRelease( m_depthStencilBuffer );
      }

      bool Init( unsigned int width, unsigned int height, bool stencilEnable );

      virtual ID3D11Resource* GetResource( ) override { return m_depthStencilBuffer; }
      virtual RenderResourceType GetResourceType( ) const override { return RenderResourceType::RDRT_DepthStencilBuffer; }
      ID3D11DepthStencilView* GetDSV( ) { return m_depthStencilView; }

      bool IsStencilEnabled( ) const { return m_bStencilEnabled; }

   private:
      ID3D11Texture2D*        m_depthStencilBuffer;
      ID3D11DepthStencilView* m_depthStencilView;
      bool                    m_bStencilEnabled;

   };
}
