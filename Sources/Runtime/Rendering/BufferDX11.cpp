#include "Rendering/BufferDX11.h"

namespace Mile
{
   BufferDX11::BufferDX11(RendererDX11* renderer) :
      ResourceDX11(renderer)
   {
      ZeroMemory(&m_desc, sizeof(D3D11_BUFFER_DESC));
   }
}
