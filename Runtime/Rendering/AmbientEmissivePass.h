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
      DEFINE_CONSTANT_BUFFER(CameraParamsConstantBuffer)
      {
         Vector3 CameraPos;
      };

      DEFINE_CONSTANT_BUFFER(AmbientParamsConstantBuffer)
      {
         float Ao;
      };

   public:
      AmbientEmissivePass(class RendererDX11* renderer);
      ~AmbientEmissivePass();

      bool Init();
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void SetGBuffer(GBuffer* gBuffer);
      void SetIrradianceMap(DynamicCubemap* irradianceMap);
      void UpdateCameraParamsBuffer(ID3D11DeviceContext& deviceContext, CameraParamsConstantBuffer buffer);
      void UpdateAmbientParamsBuffer(ID3D11DeviceContext& deviceContext, AmbientParamsConstantBuffer buffer);

   private:
      GBuffer* m_gBuffer;
      DynamicCubemap* m_irradianceMap;
      CBufferPtr m_cameraParamsBuffer;
      CBufferPtr m_ambientParamsBuffer;

   };
}