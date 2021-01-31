#pragma once
#include "Rendering/RenderingPass.h"
#include <array>
namespace Mile
{
   class BlendState;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class MEAPI GaussianBlurPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(BlurParameters)
      {
         unsigned int Horizontal;
      };

   public:
      GaussianBlurPass(class RendererDX11* renderer);
      virtual ~GaussianBlurPass();

      bool Init(unsigned int width, unsigned int height, DepthStencilBufferDX11* globalDepthStencilBuffer);
      bool Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* hdrBuffer);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      bool SwapBuffers(ID3D11DeviceContext& deviceContext, bool horizontal);

      void UpdateParameters(ID3D11DeviceContext& deviceContext, BlurParameters buffer);

      RenderTargetDX11* GetOutputBuffer() const { return m_outputHDRBuffer; }

   private:
      RenderTargetDX11* m_boundHdrBuffer;
      std::array<RenderTargetDX11*, 2> m_pingPongBuffer;
      RenderTargetDX11* m_outputHDRBuffer; /** Latest bound render target */
      CBufferPtr m_params;

   };
}