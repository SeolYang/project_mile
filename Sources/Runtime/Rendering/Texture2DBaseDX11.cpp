#include "Rendering/Texture2DBaseDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   Texture2DBaseDX11::Texture2DBaseDX11(RendererDX11* renderer) :
      m_mipLevels(1),
      m_width(0),
      m_height(0),
      ResourceDX11(renderer)
   {
   }

   void Texture2DBaseDX11::GenerateMips(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         if (m_srv != nullptr)
         {
            deviceContext.GenerateMips(m_srv);
         }
      }
   }

   bool Texture2DBaseDX11::InitSRV(D3D11_TEXTURE2D_DESC desc, bool bIsCubemap)
   {
      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
      ZeroMemory(&srvDesc, sizeof(srvDesc));
      srvDesc.Format = desc.Format;
      if (bIsCubemap)
      {
         srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
         srvDesc.TextureCube.MipLevels = -1;
         srvDesc.TextureCube.MostDetailedMip = 0;
      }
      else
      {
         srvDesc.Texture2D.MipLevels = m_mipLevels;
         srvDesc.Texture2D.MostDetailedMip = 0;
         srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      }

      return ResourceDX11::InitShaderResourceView(srvDesc);
   }
}
