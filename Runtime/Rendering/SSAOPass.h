#pragma once
#include "Rendering/RenderingPass.h"

namespace Mile
{
   class BlendState;
   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class GBuffer;
   class MEAPI SSAOPass : public RenderingPass
   {
   public:
      DEFINE_CONSTANT_BUFFER(SSAOParams)
      {
         Vector4 Samples[64];
         Vector2 NoiseScale;
      };

      DEFINE_CONSTANT_BUFFER(SSAOVariableParams)
      {
         Matrix Projection;
         float Radius;
         float Bias;
         float Magnitude;
      };

   public:
      SSAOPass(class RendererDX11* renderer);
      ~SSAOPass();

      bool Init(unsigned int width, unsigned int height);
      bool Bind(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer, Texture2dDX11* noiseTexture);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void UpdateParameters(ID3D11DeviceContext& deviceContext, SSAOParams buffer);
      void UpdateVariableParams(ID3D11DeviceContext& deviceContext, SSAOVariableParams buffer);

      RenderTargetDX11* GetOutputBuffer() const { return m_outputBuffer; }

   private:
      DepthStencilBufferDX11* m_depthStencilBuffer;
      GBuffer* m_boundGBuffer;
      Texture2dDX11* m_boundNoise;
      RenderTargetDX11* m_outputBuffer;
      CBufferPtr m_params;
      CBufferPtr m_variableParams;

   };
}