#pragma once
#include "Rendering/RenderingPass.h"
#include <array>
namespace Mile
{
   class BlendState;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class MEAPI GaussianBloomPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(BloomParameters)
      {
         bool Horizontal;
      };

   public:
      GaussianBloomPass(class RendererDX11* renderer);
      ~GaussianBloomPass();

      bool Init(unsigned int width, unsigned int height);
      bool Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* hdrBuffer);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      bool SwapBuffers(ID3D11DeviceContext& deviceContext, bool horizontal);

      void UpdateParameters(ID3D11DeviceContext& deviceContext, BloomParameters buffer);

      RenderTargetDX11* GetOutputBuffer() const { return m_outputHDRBuffer; }

   private:
      DepthStencilBufferDX11* m_depthStencilBuffer;
      RenderTargetDX11* m_boundHdrBuffer;
      std::array<RenderTargetDX11*, 2> m_pingPongBuffer;
      RenderTargetDX11* m_outputHDRBuffer; /** Latest bound render target */
      CBufferPtr m_params;

   };
}