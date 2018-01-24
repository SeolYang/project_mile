#pragma once

#include "RendererDX11.h"

namespace Mile
{
   class VertexShaderDX11;
   class PixelShaderDX11;
   class ConstantBufferDX11;
   class Texture2dDX11;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   using CBufferPtr = ConstantBufferDX11*;
   
   // If bind once, you must call unbind before bind other rendering pass.
   class MEAPI RenderingPass
   {
   public:
      RenderingPass( RendererDX11* renderer );
      virtual ~RenderingPass( );

      virtual bool Init( const String& shaderPath );

      virtual bool Bind( ID3D11DeviceContext& deviceContext );
      virtual void Unbind( ID3D11DeviceContext& deviceContext );
      
   private:
      bool InitVS( const String& filePath );
      bool InitPS( const String& filePath );

   protected:
      RendererDX11*     m_renderer;
      VertexShaderDX11* m_vertexShader;
      PixelShaderDX11*  m_pixelShader;

   };
}