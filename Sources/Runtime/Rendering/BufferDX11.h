#pragma once
#include "Rendering/ResourceDX11.h"

namespace Mile
{
   class MEAPI BufferDX11 : public ResourceDX11
   {
   public:
      BufferDX11(RendererDX11* renderer);
      D3D11_BUFFER_DESC GetDesc() const { return m_desc; }

   protected:
      D3D11_BUFFER_DESC m_desc;

   };
}