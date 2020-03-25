#include "Rendering/BufferDX11.h"

namespace Mile
{
   ID3D11Resource* BufferDX11::GetResource()
   {
      return m_buffer;
   }


   bool BufferDX11::UnMapImmediately()
   {
      if (!m_bIsInitialized || m_renderer == nullptr || !m_bIsMapped)
      {
         return false;
      }

      auto immediateContext = m_renderer->GetImmediateContext();
      if (immediateContext == nullptr)
      {
         return false;
      }

      return UnMap(*immediateContext);
   }
}
