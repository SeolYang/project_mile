#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Vector3.h"

namespace Mile
{
   constexpr float TONE_MAPPING_FACTOR = 0.3f;
   constexpr float TONE_MAPPING_GAMMA_FACTOR = 2.2f;

   class BlendState;
   class RenderTargetDX11;
   class MEAPI ToneMappingPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(ToneMappingParams)
      {
         float ToneMappingFactor = 0.4f;
         float GammaFactor = 2.2f;
      };

   public:
      ToneMappingPass(class RendererDX11* renderer);
      ~ToneMappingPass();

      bool Init();
      bool Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* hdrBuffer);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void UpdateParameters(ID3D11DeviceContext& deviceContext, ToneMappingParams buffer);

   private:
      RenderTargetDX11* m_boundHdrBuffer;
      CBufferPtr m_params;

   };
}