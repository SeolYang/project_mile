#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Vector3.h"

namespace Mile
{
   class BlendState;
   class RenderTargetDX11;
   class MEAPI ToneMappingPass : public RenderingPass
   {
   public:
      ToneMappingPass(class RendererDX11* renderer);
      ~ToneMappingPass();

      bool Init();
      bool Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* hdrBuffer);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

   private:
      RenderTargetDX11* m_boundHdrBuffer;

   };
}