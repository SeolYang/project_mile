#include "Rendering/IndexBufferDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   IndexBufferDX11::IndexBufferDX11(RendererDX11* renderer) :
      BufferDX11(renderer)
   {
   }

   bool IndexBufferDX11::Init(const std::vector<unsigned int>& indicies)
   {
      if (RenderObject::IsInitializable())
      {
         D3D11_BUFFER_DESC desc;
         ZeroMemory(&desc, sizeof(desc));
         desc.ByteWidth = static_cast<unsigned int>(sizeof(unsigned int) * indicies.size());
         desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
         desc.StructureByteStride = 0;
         desc.MiscFlags = 0;
         desc.Usage = D3D11_USAGE_IMMUTABLE;
         desc.CPUAccessFlags = 0;

         D3D11_SUBRESOURCE_DATA subResource;
         ZeroMemory(&subResource, sizeof(subResource));
         subResource.pSysMem = indicies.data();

         RendererDX11* renderer = GetRenderer();
         auto result = renderer->GetDevice().CreateBuffer(&desc, &subResource, &m_buffer);
         if (!FAILED(result))
         {
            m_desc = desc;
            RenderObject::ConfirmInit();
            return true;
         }
      }

      return false;
   }

   bool IndexBufferDX11::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         deviceContext.IASetIndexBuffer(m_buffer, DXGI_FORMAT_R32_UINT, 0);
         return true;
      }

      return false;
   }
}