#include "Rendering/VertexBufferDX11.h"

namespace Mile
{
   bool VertexBufferDX11::Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot)
   {
      if (!m_bIsInitialized)
      {
         return false;
      }

      unsigned int offset = 0;
      deviceContext.IASetVertexBuffers(startSlot, 1, &m_buffer, &m_stride, &offset);

      return true;
   }
}