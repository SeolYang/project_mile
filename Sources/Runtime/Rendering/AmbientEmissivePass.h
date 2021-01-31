#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Vector3.h"

namespace Mile
{
   class GBuffer;
   class BlendState;
   class DynamicCubemap;
   class RenderTargetDX11;
   class MEAPI AmbientEmissivePass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(AmbientParamsConstantBuffer)
      {
         Vector3 CameraPos;
         float Ao;
         unsigned int SSAOEnabled;
      };

   public:
      AmbientEmissivePass(class RendererDX11* renderer);
      virtual ~AmbientEmissivePass();

      bool Init();
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void SetGBuffer(GBuffer* gBuffer);
      void SetIrradianceMap(DynamicCubemap* irradianceMap);
      void SetPrefilteredMap(DynamicCubemap* prefilteredEnvMap);
      void SetBRDFLUT(RenderTargetDX11* brdfLUT);
      void UpdateAmbientParamsBuffer(ID3D11DeviceContext& deviceContext, AmbientParamsConstantBuffer buffer);

   private:
      GBuffer* m_gBuffer;
      DynamicCubemap* m_irradianceMap;
      DynamicCubemap* m_prefilteredMap;
      RenderTargetDX11* m_brdfLUT;
      CBufferPtr m_ambientParamsBuffer;

   };
}