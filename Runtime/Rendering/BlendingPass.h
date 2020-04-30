#pragma once
#include "Rendering/RenderingPass.h"

namespace Mile
{
   class BlendState;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class MEAPI BlendingPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(BlendingParams)
      {
         float SrcRatio = 1.0f;
         float DestRatio = 1.0f;
      };

   public:
      BlendingPass(class RendererDX11* renderer);
      ~BlendingPass();

      bool Init(unsigned int width, unsigned int height);
      bool Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* srcBuffer, RenderTargetDX11* destBuffer);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void UpdateParameters(ID3D11DeviceContext& deviceContext, BlendingParams buffer);

      RenderTargetDX11* GetOutputBuffer() const { return m_outputHDRBuffer; }

   private:
      DepthStencilBufferDX11* m_depthStencilBuffer;
      RenderTargetDX11* m_boundSrcBuffer;
      RenderTargetDX11* m_boundDestBuffer;
      RenderTargetDX11* m_outputHDRBuffer;
      CBufferPtr m_params;

   };
}