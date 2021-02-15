#pragma once
#include "Rendering/RenderObject.h"

#define DEFINE_CONSTANT_BUFFER(BUFFER_NAME) struct alignas(16) BUFFER_NAME
namespace Mile
{
   class VertexShaderDX11;
   class PixelShaderDX11;
   class ConstantBufferDX11;
   class Texture2dDX11;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   using CBufferPtr = ConstantBufferDX11 *;

   /**
    * @brief	렌더러에서 최종 결과물을 만들어 내기 위해 실행될 각 단계들을 나타내는 클래스 입니다.
   * @todo		다른 셰이더들(GS, Tessellation stage, CS.. )도 렌더링 패스에서 사용할수 있도록 하기.
   * @todo		각 셰이더들의 file path 분리 (셰이버 파일내부에서 include 하는 방법도 고려)
    */
   class MEAPI RenderingPass : public RenderObject
   {
   public:
      RenderingPass(RendererDX11* renderer);
      virtual ~RenderingPass();

      virtual bool Init(const String& shaderPath);

      /**
       * @brief   로드된 셰이더들만 바인드 합니다. 
       *          (모든 서브 클래스들은 Bind 시 RenderingPass::Bind 를 호출하여야 합니다.)
       */
      virtual bool Bind(ID3D11DeviceContext& deviceContext);

      /**
       * @brief   로드된 셰이더들을 언바인드 합니다.
       *          (모든 서브 클래스들은 Unbind 시 RenderingPass::Unbind 를 호출하여야 합니다.)
       */
      virtual void Unbind(ID3D11DeviceContext& deviceContext);

      FORCEINLINE VertexShaderDX11* GetVertexShader() const { return m_vertexShader; }
      FORCEINLINE PixelShaderDX11* GetPixelShader() const { return m_pixelShader; }

      void SetClearStateWhenBind(bool bClearStateWhenBind) { m_bClearStateWhenBind = bClearStateWhenBind; }
      void SetPrimitiveTopology(EPrimitiveTopology primitiveTopology) { m_primitiveTopology = primitiveTopology; }

   private:
      bool InitVS(const String& filePath);
      bool InitPS(const String& filePath);

   private:
      VertexShaderDX11* m_vertexShader;
      PixelShaderDX11* m_pixelShader;

      bool m_bClearStateWhenBind;
      EPrimitiveTopology m_primitiveTopology;

   };
}