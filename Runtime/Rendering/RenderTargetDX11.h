#pragma once

#include "RendererDX11.h"

namespace Mile
{
   class Texture2dDX11;
   class DepthStencilBufferDX11;
   class MEAPI RenderTargetDX11
   {
   public:
      RenderTargetDX11( RendererDX11* renderer );
      virtual ~RenderTargetDX11( );
      
      bool Init( unsigned int width, unsigned int height, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM );

      unsigned int GetWidth( ) const { return m_width; }
      unsigned int GetHeight( ) const { return m_height; }

      Texture2dDX11* GetTexture( ) const { return m_texture; }
      ID3D11RenderTargetView* GetRTV( ) const { return m_rtv; }

      void SetDepthStencilBuffer( DepthStencilBufferDX11* buffer ) { this->m_depthStencilBuffer = buffer; }

      bool BindAsRenderTarget( );
      bool BindAsShaderResource( unsigned int startSlot, ShaderType shader );
      void UnbindRenderTarget( );
      void UnbindShaderResource( );

   private:
      RendererDX11*              m_renderer;
      Texture2dDX11*             m_texture;
      ID3D11RenderTargetView*    m_rtv;

      unsigned int               m_width;
      unsigned int               m_height;

      DepthStencilBufferDX11*    m_depthStencilBuffer;

   };
}
