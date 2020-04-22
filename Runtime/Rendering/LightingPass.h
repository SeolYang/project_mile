#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Vector3.h"

namespace Mile
{
   class GBuffer;
   class BlendState;
   class MEAPI LightingPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(CameraParamsConstantBuffer)
      {
         Vector3 CameraPos;
      };

      DEFINE_CONSTANT_BUFFER(LightParamsConstantBuffer)
      {
         Vector4 LightPos;
         Vector4 LightDirection;
         Vector4 LightRadiance;
         UINT32 LightType;
      };

   public:
      LightingPass(class RendererDX11* renderer);
      ~LightingPass();

      bool Init();
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void SetGBuffer(GBuffer* gBuffer);
      void UpdateCameraParamsBuffer(ID3D11DeviceContext& deviceContext, CameraParamsConstantBuffer buffer);
      void UpdateLightParamsBuffer(ID3D11DeviceContext& deviceContext, LightParamsConstantBuffer buffer);

   private:
      GBuffer* m_gBuffer;
      CBufferPtr m_cameraParamsBuffer;
      CBufferPtr m_lightParamsBuffer;

   };
}