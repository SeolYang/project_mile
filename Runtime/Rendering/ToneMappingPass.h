#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace Mile
{
   class BlendState;
   class RenderTargetDX11;
   class MEAPI ToneMappingPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(ToneMappingParams)
      {
         float ExposureFactor = 0.4f;
         float GammaFactor = 2.2f;
      };

      DEFINE_CONSTANT_BUFFER(PackedToneMappingParams)
      {
         Vector2 Factors;
      };

   public:
      ToneMappingPass(class RendererDX11* renderer);
      virtual ~ToneMappingPass();

      bool Init();
      bool Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* hdrBuffer);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void UpdateParameters(ID3D11DeviceContext& deviceContext, ToneMappingParams buffer);

   private:
      RenderTargetDX11* m_boundHdrBuffer;
      CBufferPtr m_params;

   };
}