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
   
   /**
    * @brief	렌더러에서 최종 결과물을 만들어 내기 위해 실행될 각 단계들을 나타내는 클래스 입니다.
	* @todo		다른 셰이더들(GS, Tessellation stage, CS.. )도 렌더링 패스에서 사용할수 있도록 하기.
	* @todo		각 셰이더들의 file path 분리 (셰이버 파일내부에서 include 하는 방법도 고려)
    */
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