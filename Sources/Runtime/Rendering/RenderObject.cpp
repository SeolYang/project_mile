#include "Rendering/RenderObject.h"
#include "Core/Engine.h"

namespace Mile
{
   RenderObject::RenderObject(RendererDX11* renderer) :
      m_renderer(renderer),
      m_bIsInitialized(false)
   {
   }

   RenderObject::~RenderObject()
   {
      m_renderer = nullptr;
      m_bIsInitialized = false;
   }

   bool RenderObject::HasAvailableRenderer() const
   {
      return (m_renderer != nullptr) && (Engine::GetRenderer() == m_renderer);
   }
}