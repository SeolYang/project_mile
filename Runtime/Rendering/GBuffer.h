#pragma once

#include "RendererDX11.h"

namespace Mile
{
   class RenderTargetDX11;
   class MEAPI GBuffer
   {
   public:
      GBuffer( RendererDX11* renderer ) :
         m_renderer( renderer ),
         m_normalBuffer( nullptr ),
         m_positionBuffer( nullptr )
      {
      }

      ~GBuffer( )
      {
         SafeDelete( m_normalBuffer );
         SafeDelete( m_positionBuffer );
      }

      bool Init( unsigned int width, unsigned int height );
      
      bool BindAsRenderTarget( );

      bool BindAsShaderResource( unsigned int startSlot );
      void Unbind( );

   private:
      RendererDX11*        m_renderer;
      RenderTargetDX11*    m_normalBuffer;
      RenderTargetDX11*    m_positionBuffer;

   };
}