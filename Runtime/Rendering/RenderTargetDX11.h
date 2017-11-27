#pragma once

#include "RendererDX11.h"

namespace Mile
{
   class Texture2dDX11;
   class MEAPI RenderTargetDX11
   {
   public:
      RenderTargetDX11( RendererDX11* renderer ) :
         m_renderer( renderer ),
         m_texture( nullptr ),
         m_rtv( nullptr ),
         m_width( 0 ),
         m_height( 0 )
      {
      }

      virtual ~RenderTargetDX11( )
      {
         SafeRelease( m_rtv );
         SafeDelete( m_texture );
      }
      
      bool Init( unsigned int width, unsigned int height );

      unsigned int GetWidth( ) const { return m_width; }
      unsigned int GetHeight( ) const { return m_height; }

      Texture2dDX11* GetTexture( ) const { return m_texture; }
      ID3D11RenderTargetView* GetRTV( ) const { return m_rtv; }

      bool BindAsRenderTarget( );
      bool BindAsShaderResource( unsigned int startSlot, ShaderType shader );
      void Unbind( );

   private:
      RendererDX11*              m_renderer;
      Texture2dDX11*             m_texture;
      ID3D11RenderTargetView*    m_rtv;

      unsigned int               m_width;
      unsigned int               m_height;

   };
}
