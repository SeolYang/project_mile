#include "Rendering/Texture2dDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   Texture2dDX11::Texture2dDX11(RendererDX11* renderer) :
      Texture2DBaseDX11(renderer)
   {
   }

   bool Texture2dDX11::Init(unsigned int width, unsigned int height, unsigned int channels, unsigned char* data, DXGI_FORMAT format)
   {
      if (RenderObject::IsInitializable())
      {
         RendererDX11* renderer = GetRenderer();
         auto& device = renderer->GetDevice();

         D3D11_TEXTURE2D_DESC desc;
         ZeroMemory(&desc, sizeof(desc));
         desc.Width = width;
         desc.Height = height;
         desc.MipLevels = m_mipLevels;
         desc.ArraySize = 1;
         desc.Format = format;
         desc.SampleDesc.Count = 1;
         desc.SampleDesc.Quality = 0;
         desc.Usage = D3D11_USAGE_IMMUTABLE;
         desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
         desc.MiscFlags = 0;
         desc.CPUAccessFlags = 0;

         unsigned int bytePerChannel = sizeof(unsigned char);
         switch (format)
         {
         case DXGI_FORMAT_R16G16B16A16_FLOAT:
            bytePerChannel = 2;
         case DXGI_FORMAT_R32G32B32A32_FLOAT:
         case DXGI_FORMAT_R32G32B32_FLOAT:
            bytePerChannel = 4;
            break;
         }

         D3D11_SUBRESOURCE_DATA resource;
         resource.pSysMem = data;
         resource.SysMemPitch = (width * channels) * bytePerChannel;
         resource.SysMemSlicePitch = (width * height * channels) * bytePerChannel;

         auto result = device.CreateTexture2D(&desc, &resource, reinterpret_cast<ID3D11Texture2D**>(&m_resource));
         if (!FAILED(result))
         {
            if (InitSRV(desc))
            {
               m_width = width;
               m_height = height;
               ResourceDX11::ConfirmInit();
               return true;
            }
         }
      }

      return false;
   }

   bool Texture2dDX11::Init(ID3D11Texture2D* texture)
   {
      bool bValidParams = texture != nullptr;
      if (RenderObject::IsInitializable() && bValidParams)
      {
         m_resource = static_cast<ID3D11Resource*>(texture);

         D3D11_TEXTURE2D_DESC desc;
         texture->GetDesc(&desc);

         m_width = desc.Width;
         m_height = desc.Height;

         if (InitSRV(desc))
         {
            ResourceDX11::ConfirmInit();
            return true;
         }
      }

      return false;
   }
}