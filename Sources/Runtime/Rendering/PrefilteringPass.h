#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Matrix.h"

namespace Mile
{
   class Texture2DBaseDX11;
   class DynamicCubemap;
   class RasterizerState;
   class Viewport;
   class MEAPI PrefilteringPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(TransformConstantBuffer)
      {
         Matrix ViewProj;
      };

      DEFINE_CONSTANT_BUFFER(PrefilteringParams)
      {
         float Roughness;
      };

   public:
      PrefilteringPass(RendererDX11* renderer);
      ~PrefilteringPass();

      bool Init(unsigned int cubemapSize);
      bool Bind(ID3D11DeviceContext& deviceContext, Texture2DBaseDX11* environmentMap);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      DynamicCubemap* GetPrefilteredEnvironmentMap() const { return m_prefilteredEnvMap; }
      Viewport* GetViewport(unsigned int mipLevel) { return m_viewports[mipLevel]; }

      void UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer);
      void UpdatePrefilteringParams(ID3D11DeviceContext& deviceContext, PrefilteringParams buffer);

   private:
      std::vector<Viewport*> m_viewports;
      DynamicCubemap* m_prefilteredEnvMap;
      Texture2DBaseDX11* m_boundEnvironmentMap;
      CBufferPtr m_transformBuffer;
      CBufferPtr m_prefilteringParams;

   };
}