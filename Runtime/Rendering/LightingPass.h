#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Vector3.h"

namespace Mile
{
   class GBuffer;
   class BlendState;
   class MEAPI LightingPass : public RenderingPass
   {
      struct alignas(16) CameraParamsConstantBuffer
      {
         Vector3 CameraPos;
      };

      struct alignas(16) LightParamsConstantBuffer
      {
         Vector3 LightPos;
         Vector3 LightRadiance;
         Vector3 LightDirection;
         UINT32 LightType;
      }

   public:
      LightingPass(class RendererDX11* renderer);
      ~LightingPass();

      virtual bool Init(const String& shaderPath) override;
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual bool Unbind(ID3D11DeviceContext& deviceContext) override;

      void SetGBuffer(GBuffer* gBuffer);
      void UpdateCameraParamsBuffer(ID3D11DeviceContext& deviceContext, CameraParamsConstantBuffer buffer);
      void UpdateLightParamsBuffer(ID3D11DeviceContext& deviceContext, LightParamsConstantBuffer buffer);

   private:
      GBuffer* m_gBuffer;
      CBufferPtr m_cameraParamsBuffer;
      CBufferPtr m_lightParamsBuffer;
      BlendState* m_additiveBlendState;

   };
}