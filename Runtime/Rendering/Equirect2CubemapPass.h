#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Matrix.h"

namespace Mile
{
   class Texture2dDX11;
   class DynamicCubemap;
   class RasterizerState;
   class Viewport;
   class MEAPI Equirect2CubemapPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(TransformConstantBuffer)
      {
         Matrix ViewProj;
      };

   public:
      Equirect2CubemapPass(RendererDX11* renderer);
      ~Equirect2CubemapPass();

      bool Init(const String& shaderPath, unsigned int cubemapSize);
      bool Bind(ID3D11DeviceContext& deviceContext, Texture2dDX11* equirectangularMap, unsigned int faceIndex);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      DynamicCubemap* GetCubemap() const { return m_cubemap; }

      void UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer);

   private:
      Viewport* m_viewport;
      DynamicCubemap* m_cubemap;
      Texture2dDX11* m_boundEquirectMap;
      CBufferPtr m_transformBuffer;

   };
}