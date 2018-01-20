#pragma once

#include "RendererDX11.h"

namespace Mile
{
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class MEAPI GBuffer
   {
   public:
      GBuffer( RendererDX11* renderer );
      ~GBuffer( );

      bool Init( unsigned int width, unsigned int height );

      void SetDepthStencilBuffer( DepthStencilBufferDX11* buffer ) { this->m_depthStencilBuffer = buffer; }
      
      bool BindAsRenderTarget( );
      bool BindAsShaderResource( unsigned int startSlot );
      void UnbindRenderTarget( );
      void UnbindShaderResource( );

   private:
      RendererDX11*        m_renderer;
      DepthStencilBufferDX11* m_depthStencilBuffer;
      RenderTargetDX11*    m_normalBuffer;
      RenderTargetDX11*    m_positionBuffer;

   };
}