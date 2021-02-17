#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   DepthStencilBufferDX11::DepthStencilBufferDX11(RendererDX11* renderer) :
      m_bStencilEnabled(true),
      m_depthStencilBuffer(nullptr),
      m_depthStencilView(nullptr),
      m_srv(nullptr),
      ResourceDX11(renderer)
   {
   }

   DepthStencilBufferDX11::~DepthStencilBufferDX11()
   {
      SafeRelease(m_srv);
      SafeRelease(m_depthStencilView);
      SafeRelease(m_depthStencilBuffer);
   }

   bool DepthStencilBufferDX11::Init(unsigned int width, unsigned int height, bool bStencilEnable)
   {
      if (RenderObject::IsInitializable())
      {
         m_bStencilEnabled = bStencilEnable;

         D3D11_TEXTURE2D_DESC bufferDesc;
         ZeroMemory(&bufferDesc, sizeof(bufferDesc));
         bufferDesc.Width = width;
         bufferDesc.Height = height;
         bufferDesc.MipLevels = 0;
         bufferDesc.ArraySize = 1;
         bufferDesc.Format = bStencilEnable ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
         bufferDesc.SampleDesc.Count = 1;
         bufferDesc.SampleDesc.Quality = 0;
         bufferDesc.Usage = D3D11_USAGE_DEFAULT;
         bufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
         bufferDesc.CPUAccessFlags = 0;
         bufferDesc.MiscFlags = 0;

         RendererDX11* renderer = GetRenderer();
         auto& device = renderer->GetDevice();
         auto result = device.CreateTexture2D(
            &bufferDesc,
            nullptr,
            &m_depthStencilBuffer);
         if (!FAILED(result))
         {
            D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
            ZeroMemory(&dsvDesc, sizeof(dsvDesc));
            dsvDesc.Format = bStencilEnable ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = 0;

            result = device.CreateDepthStencilView(m_depthStencilBuffer, &dsvDesc, &m_depthStencilView);
            if (!FAILED(result))
            {
               D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
               ZeroMemory(&srvDesc, sizeof(srvDesc));
               srvDesc.Format = bStencilEnable ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT;
               srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
               srvDesc.Texture2D.MipLevels = -1;
               srvDesc.Texture2D.MostDetailedMip = 0;

               result = device.CreateShaderResourceView(m_depthStencilBuffer, &srvDesc, &m_srv);
               if (!FAILED(result))
               {
                  RenderObject::ConfirmInit();
                  return true;
               }
            }
            else
            {
               /* Failed to create Depth-Stencil View. **/
            }
         }
         else
         {
            /* Failed to create Depth-Stencil Buffer. **/
         }
      }

      return false;
   }
}