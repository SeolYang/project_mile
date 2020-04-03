#pragma once
#include "Rendering/BufferDX11.h"

namespace Mile
{
   class MEAPI IndexBufferDX11 : public BufferDX11
   {
   public:
      IndexBufferDX11(RendererDX11* renderer);

      bool Init(const std::vector<unsigned int>& indicies);
      bool Bind(ID3D11DeviceContext& deviceContext);

      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::IndexBuffer; }

   };
}