#pragma once
#include "Rendering/RenderingPass.h"

namespace Mile
{
   class BlendState;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class MEAPI BoxBloomPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(BloomParameters)
      {
         int Size = 3;
         float Separation = 5.0f;
         float Threshold = 0.8f;
         float Amount = 0.5f;
      };

   public:
      BoxBloomPass(class RendererDX11* renderer);
      virtual ~BoxBloomPass();

      bool Init(unsigned int width, unsigned int height, DepthStencilBufferDX11* globalDepthStencilBuffer);
      bool Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* hdrBuffer);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void UpdateParameters(ID3D11DeviceContext& deviceContext, BloomParameters buffer);

      RenderTargetDX11* GetOutputBuffer() const { return m_outputHDRBuffer; }

   private:
      RenderTargetDX11* m_boundHdrBuffer;
      RenderTargetDX11* m_outputHDRBuffer;
      CBufferPtr m_params;

   };
}