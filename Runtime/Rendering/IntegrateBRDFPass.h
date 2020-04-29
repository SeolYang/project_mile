#pragma once
#include "Rendering/RenderingPass.h"

namespace Mile
{
   constexpr unsigned int DEFAULT_BRDF_LUT_SIZE = 512;

   class BlendState;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class Viewport;
   class MEAPI IntegrateBRDFPass : public RenderingPass
   {
   public:
      IntegrateBRDFPass(class RendererDX11* renderer);
      ~IntegrateBRDFPass();

      bool Init(unsigned int lutSize = DEFAULT_BRDF_LUT_SIZE);
      bool Bind(ID3D11DeviceContext& deviceContext);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      RenderTargetDX11* GetBrdfLUT() const { return m_brdfLUT; }

   private:
      Viewport* m_viewport;
      RenderTargetDX11* m_brdfLUT;
      DepthStencilBufferDX11* m_depthStencil;

   };
}