#pragma once
#include "Rendering/RenderingPass.h"

namespace Mile
{
   class BlendState;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class GBuffer;
   class MEAPI SSAOBlurPass : public RenderingPass
   {
   public:
      SSAOBlurPass(class RendererDX11* renderer);
      ~SSAOBlurPass();

      bool Init(unsigned int width, unsigned int height, DepthStencilBufferDX11* globalDepthStencilBuffer);
      bool Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* ssaoInput);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      RenderTargetDX11* GetOutputBuffer() const { return m_outputBuffer; }

   private:
      RenderTargetDX11* m_boundSSAOInput;
      RenderTargetDX11* m_outputBuffer;

   };
}