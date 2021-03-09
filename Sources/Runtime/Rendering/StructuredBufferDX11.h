#pragma once
#include "Rendering/BufferDX11.h"

namespace Mile
{
   class MEAPI StructuredBufferDX11 : public BufferDX11
   {
   public:
      StructuredBufferDX11(RendererDX11* renderer) : BufferDX11(renderer)
      {
      }

      bool Init(unsigned int count, unsigned int structSize, bool bCPUWritable, bool bGPUWritable, D3D11_SUBRESOURCE_DATA* data);

      template <typename Struct>
      bool Init(unsigned int count, bool bCPUWritable, bool bGPUWritable, D3D11_SUBRESOURCE_DATA* data)
      {
         return Init(count, sizeof(Struct), bCPUWritable, bGPUWritable, data);
      }

      ERenderResourceType GetResourceType() const override { return ERenderResourceType::StructuredBuffer; }

   };
}