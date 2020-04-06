#pragma once
#include "Rendering/RendererDX11.h"
#include "Core/Engine.h"

namespace Mile
{
   enum class ERenderResourceType
   {
      VertexBuffer,
      IndexBuffer,
      ConstantBuffer,
      StructuredBuffer,
      ByteAddressBuffer,
      IndirectArgumentsBuffer,
      Texture1D,
      Texture2D,
      Texture3D,
      RenderTarget,
      DepthStencilBuffer,
      DynamicCubemap
   };

   class RendererDX11;
   class MEAPI ResourceDX11
   {
   public:
      ResourceDX11(RendererDX11* renderer) :
         m_bIsInitialized(false),
         m_renderer(renderer)
      {
      }

      virtual ~ResourceDX11()
      {
         m_renderer = nullptr;
      }

      virtual ID3D11Resource* GetResource() const = 0;
      virtual ERenderResourceType GetResourceType() const = 0;

      FORCEINLINE bool IsInitialized() const { return m_bIsInitialized; }
      FORCEINLINE bool HasAvailableRenderer() const
      {
         return (m_renderer != nullptr) && (Engine::GetRenderer() == m_renderer);
      }

      FORCEINLINE RendererDX11* GetRenderer() const { return m_renderer; }

   protected:
      FORCEINLINE void ConfirmInitialize() { m_bIsInitialized = true; }

   private:
      RendererDX11* m_renderer;
      bool m_bIsInitialized;

   };
}