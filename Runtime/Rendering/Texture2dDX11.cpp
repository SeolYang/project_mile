#include "Rendering/Texture2dDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   Texture2dDX11::Texture2dDX11(RendererDX11* renderer) :
      Texture2DBaseDX11(renderer)
   {
   }

   Texture2dDX11::~Texture2dDX11()
   {
   }

   bool Texture2dDX11::Init(unsigned int width, unsigned int height, int channels, unsigned char* data, DXGI_FORMAT format)
   {
      if (IsPreparedToInitialize())
      {
         RendererDX11* renderer = GetRenderer();
         auto device = renderer->GetDevice();

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

         D3D11_SUBRESOURCE_DATA resource;
         resource.pSysMem = data;
         resource.SysMemPitch = (width * channels) * sizeof(unsigned char);
         resource.SysMemSlicePitch = (width * height * channels) * sizeof(unsigned char);

         auto result = device->CreateTexture2D(&desc, &resource, &m_texture);
         if (!FAILED(result))
         {
            if (InitSRV(desc))
            {
               ResourceDX11::ConfirmInitialize();
               return true;
            }
            else
            {
               /* Failed to Init Shader Resource View! **/
               SafeRelease(m_texture);
            }
         }
      }

      return false;
   }

   bool Texture2dDX11::Init(ID3D11Texture2D* texture)
   {
      if (texture != nullptr && IsPreparedToInitialize())
      {
         m_texture = texture;

         D3D11_TEXTURE2D_DESC desc;
         m_texture->GetDesc(&desc);

         m_width = desc.Width;
         m_height = desc.Height;

         if (InitSRV(desc))
         {
            ResourceDX11::ConfirmInitialize();
            return true;
         }
         else
         {
            SafeRelease(m_texture);
         }
      }

      return false;
   }

   bool Texture2dDX11::InitSRV(D3D11_TEXTURE2D_DESC desc)
   {
      bool IsReadyToInitSRV = (m_srv == nullptr) && HasAvailableRenderer();
      if (IsReadyToInitSRV)
      {
         RendererDX11* renderer = GetRenderer();
         auto device = renderer->GetDevice();
         D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
         ZeroMemory(&srvDesc, sizeof(srvDesc));
         srvDesc.Texture2D.MipLevels = m_mipLevels;
         srvDesc.Texture2D.MostDetailedMip = 0;
         srvDesc.Format = desc.Format;
         srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

         auto result = device->CreateShaderResourceView(m_texture, &srvDesc, &m_srv);
         if (!FAILED(result))
         {
            return true;
         }
      }

      return false;
   }
}