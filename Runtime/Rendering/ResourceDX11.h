#pragma once
#include "Rendering/RenderObject.h"

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
   class MEAPI ResourceDX11 : public RenderObject
   {
   public:
      ResourceDX11(RendererDX11* renderer) :
         RenderObject(renderer)
      {
      }

      virtual ~ResourceDX11()
      {
      }

      virtual ID3D11Resource* GetResource() const = 0;
      virtual ERenderResourceType GetResourceType() const = 0;

   };
}