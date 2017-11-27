#pragma once

#include "RendererDX11.h"

namespace Mile
{
   class VertexShaderDX11;
   class PixelShaderDX11;
   class ConstantBufferDX11;
   class MEAPI RenderingPass
   {
   public:
      RenderingPass( RendererDX11* renderer );
      virtual ~RenderingPass( );

      virtual bool Init( const String& shaderPath );

      virtual bool Bind( );
      virtual void Unbind( );
      
   private:
      bool InitVS( const String& filePath );
      bool InitPS( const String& filePath );

   protected:
      RendererDX11*     m_renderer;
      VertexShaderDX11* m_vertexShader;
      PixelShaderDX11*  m_pixelShader;

   };
}