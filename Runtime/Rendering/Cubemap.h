#pragma once
#include "Rendering/ResourceDX11.h"

namespace Mile
{
   class MEAPI Cubemap : public ResourceDX11
   {
   public:
      Cubemap(RendererDX11* renderer);
      ~Cubemap();

      virtual ID3D11Resource* GetResource() const override;

   private:
      ID3D11Texture2D* m_texture;
      ID3D11ShaderResourceView* m_srv;

   };
}