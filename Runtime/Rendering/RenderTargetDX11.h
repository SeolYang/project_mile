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
      
      bool Init( unsigned int width, unsigned int height, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, DepthStencilBufferDX11* depthStencilBuffer = nullptr );
      /* If initialize with this method, it is not able to bind as shader resource. Because it suppose always bounded as Render target view */
      bool Init( ID3D11RenderTargetView* rtv, DepthStencilBufferDX11* depthStencilBuffer = nullptr );

      unsigned int GetWidth( ) const { return m_width; }
      unsigned int GetHeight( ) const { return m_height; }

      Texture2dDX11* GetTexture( ) const { return m_texture; }
      ID3D11RenderTargetView* GetRTV( ) const { return m_rtv; }

      void SetDepthStencilBuffer( DepthStencilBufferDX11* buffer ) { this->m_depthStencilBuffer = buffer; }

      bool BindAsRenderTarget( ID3D11DeviceContext& deviceContext );
      bool BindAsShaderResource( ID3D11DeviceContext& deviceContext, unsigned int startSlot, ShaderType shader );
      void UnbindRenderTarget( ID3D11DeviceContext& deviceContext );
      void UnbindShaderResource( ID3D11DeviceContext& deviceContext );

      void SetClearColor( const Vector4& color );

   private:
      RendererDX11*              m_renderer;
      Texture2dDX11*             m_texture;
      ID3D11RenderTargetView*    m_rtv;

      unsigned int               m_width;
      unsigned int               m_height;

      DepthStencilBufferDX11*    m_depthStencilBuffer;

      Vector4                    m_clearColor;

   };
}
