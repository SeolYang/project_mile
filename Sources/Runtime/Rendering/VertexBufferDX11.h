#pragma once
#include "Rendering/BufferDX11.h"

namespace Mile
{
   class MEAPI VertexBufferDX11 : public BufferDX11
   {
   public:
      VertexBufferDX11(RendererDX11* renderer);

      template < typename VertexTy >
      bool Init(const std::vector<VertexTy>& verticies)
      {
         if (RenderObject::IsInitializable())
         {
            m_stride = sizeof(VertexTy);

            D3D11_BUFFER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            desc.ByteWidth = static_cast<unsigned int>(sizeof(VertexTy) * verticies.size());
            desc.StructureByteStride = 0;
            desc.MiscFlags = 0;
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            desc.CPUAccessFlags = 0;

            D3D11_SUBRESOURCE_DATA subResource;
            ZeroMemory(&subResource, sizeof(subResource));
            subResource.pSysMem = verticies.data();

            RendererDX11* renderer = GetRenderer();
            auto result = renderer->GetDevice().CreateBuffer(&desc, &subResource, &m_buffer);
            if (!FAILED(result))
            {
               ResourceDX11::ConfirmInit();
               return true;
            }
         }

         return false;
      }

      bool Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot);

      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::VertexBuffer; }
      FORCEINLINE unsigned int GetStride() const { return m_stride; }

   private:
      unsigned int m_stride;

   };
}