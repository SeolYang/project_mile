#include "Rendering/BufferDX11.h"

namespace Mile
{
   BufferDX11::BufferDX11(RendererDX11* renderer) :
      m_bIsMapped(false),
      m_buffer(nullptr),
      ResourceDX11(renderer)
   {
      ZeroMemory(&m_desc, sizeof(D3D11_BUFFER_DESC));
   }

   BufferDX11::~BufferDX11()
   {
      SafeRelease(m_buffer);
   }

   bool BufferDX11::UnMapImmediately()
   {
      bool bIsReadyToUnmap = m_bIsMapped && HasAvailableRenderer() && IsInitialized();
      if (bIsReadyToUnmap)
      {
         RendererDX11* renderer = GetRenderer();
         auto immediateContext = renderer->GetImmediateContext();
         if (immediateContext != nullptr)
         {
            return UnMap(*immediateContext);
         }
      }

      return false;
   }
}
