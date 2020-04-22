#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Matrix.h"

namespace Mile
{
   class Texture2DBaseDX11;
   class DepthStencilState;
   class MEAPI SkyboxPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(TransformConstantBuffer)
      {
         Matrix ViewProj;
      };

   public:
      SkyboxPass(RendererDX11* renderer);
      ~SkyboxPass();

      bool Init(const String& shaderPath);
      bool Bind(ID3D11DeviceContext& deviceContext, Texture2DBaseDX11* environmentMap);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer);

   private:
      DepthStencilState* m_depthStencilState;
      Texture2DBaseDX11* m_boundEnvironmentMap;
      CBufferPtr m_transformBuffer;

   };
}