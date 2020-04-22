#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Matrix.h"

namespace Mile
{
   class Texture2DBaseDX11;
   class DynamicCubemap;
   class RasterizerState;
   class Viewport;
   class MEAPI IrradianceConvPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(TransformConstantBuffer)
      {
         Matrix ViewProj;
      };

   public:
      IrradianceConvPass(RendererDX11* renderer);
      ~IrradianceConvPass();

      bool Init(unsigned int cubemapSize);
      bool Bind(ID3D11DeviceContext& deviceContext, Texture2DBaseDX11* environmentMap);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      DynamicCubemap* GetIrradianceMap() const { return m_irradianceMap; }

      void UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer);

   private:
      Viewport* m_viewport;
      DynamicCubemap* m_irradianceMap;
      Texture2DBaseDX11* m_boundEnvironmentMap;
      CBufferPtr m_transformBuffer;

   };
}