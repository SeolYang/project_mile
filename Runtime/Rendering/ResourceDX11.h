#pragma once
#include "Rendering/RendererDX11.h"

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
      Cubemap
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
      }

      virtual ID3D11Resource* GetResource() const = 0;
      virtual ERenderResourceType GetResourceType() const = 0;

      bool IsInitialized() const { return m_bIsInitialized; }

   protected:
      RendererDX11* m_renderer;
      bool m_bIsInitialized;

   };
}