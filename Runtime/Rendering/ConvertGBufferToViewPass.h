#pragma once
#include "Rendering/RenderingPass.h"

namespace Mile
{
   class BlendState;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class GBuffer;
   class MEAPI ConvertGBufferToViewPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(ConvertParams)
      {
         Matrix View;
      };

   public:
      ConvertGBufferToViewPass(class RendererDX11* renderer);
      ~ConvertGBufferToViewPass();

      bool Init(unsigned int width, unsigned int height, DepthStencilBufferDX11* globalDepthStencilBuffer);
      bool Bind(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void UpdateParameters(ID3D11DeviceContext& deviceContext, ConvertParams buffer);

      GBuffer* GetConvertedGBuffer() const { return m_convertedGBuffer; }

   private:
      GBuffer* m_boundGBuffer;
      GBuffer* m_convertedGBuffer;
      CBufferPtr m_params;

   };
}