#pragma once

#include "ResourceDX11.h"

namespace Mile
{
   class MEAPI RenderTargetDX11 : public ResourceDX11
   {
   public:
      RenderTargetDX11( RendererDX11* renderer ) :
         m_renderTarget( nullptr ),
         m_renderTargetView( nullptr ),
         m_srv( nullptr ),
         m_width( 0 ),
         m_height( 0 ),
         ResourceDX11( renderer )
      {
      }

      virtual ~RenderTargetDX11( )
      {
         SafeRelease( m_srv );
         SafeRelease( m_renderTargetView );
         SafeRelease( m_renderTarget );
      }
      
      bool Init( unsigned int width, unsigned int height );

      unsigned int GetWidth( ) const { return m_width; }
      unsigned int GetHeight( ) const { return m_height; }

      virtual ID3D11Resource* GetResource( ) override { return m_renderTarget; }
      virtual RenderResourceType GetResourceType( ) const override { return RenderResourceType::RDRT_RenderTarget; }
      ID3D11ShaderResourceView* GetSRV( ) { return m_srv; }

      bool SetAsRenderTarget( );

   private:
      ID3D11Texture2D*           m_renderTarget;
      ID3D11RenderTargetView*    m_renderTargetView;
      ID3D11ShaderResourceView*  m_srv;

      unsigned int               m_width;
      unsigned int               m_height;
   };
}
