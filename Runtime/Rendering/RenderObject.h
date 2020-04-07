#pragma once
#include "Rendering/RenderingCore.h"
#include "Core/Engine.h"

namespace Mile
{
   class RendererDX11;
   class MEAPI RenderObject
   {
   public:
      RenderObject(RendererDX11* renderer);
      virtual ~RenderObject();

      FORCEINLINE bool IsInitialized() const { return m_bIsInitialized; }
      bool HasAvailableRenderer() const;
      FORCEINLINE bool IsInitializable() const { return (!IsInitialized() && HasAvailableRenderer()); }
      FORCEINLINE bool IsBindable() const { return (IsInitialized() && HasAvailableRenderer()); }
      FORCEINLINE RendererDX11* GetRenderer() const { return m_renderer; }

   protected:
      FORCEINLINE void ConfirmInit() { m_bIsInitialized = true; }

   private:
      RendererDX11* m_renderer;
      bool m_bIsInitialized;

   };
}