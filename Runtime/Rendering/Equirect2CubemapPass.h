#pragma once
#include "Rendering/RenderingPass.h"

namespace Mile
{
   class Texture2dDX11;
   class DynamicCubemap;
   class MEAPI Equirect2CubemapPass : public RenderingPass
   {
   public:
      Equirect2CubemapPass(RendererDX11* renderer);
      ~Equirect2CubemapPass();

      bool Init(const String& shaderPath, unsigned int cubemapSize);
      bool Bind(ID3D11DeviceContext& deviceContext, Texture2dDX11* equirectangularMap);
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

   private:
      DynamicCubemap* m_cubemap;

   };
}