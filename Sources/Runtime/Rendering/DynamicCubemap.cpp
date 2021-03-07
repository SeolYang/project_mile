#include "Rendering/DynamicCubemap.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   DynamicCubemap::DynamicCubemap(RendererDX11* renderer) :
      m_rtvs(),
      m_maxMipLevels(0),
      Texture2DBaseDX11(renderer)
   {
   }

   DynamicCubemap::~DynamicCubemap()
   {
      for (unsigned int idx = 0; idx < 6; ++idx)
      {
         for (unsigned int mipLevel = 0; mipLevel <= m_maxMipLevels; ++mipLevel)
         {
            SafeRelease(m_rtvs[idx][mipLevel]);
         }
      }
   }
   
   bool DynamicCubemap::Init(unsigned int size)
   {
      if (RenderObject::IsInitializable())
      {
         m_maxMipLevels = static_cast<unsigned int>(std::log2f(static_cast<float>(size)));
         RendererDX11* renderer = GetRenderer();
         auto& device = renderer->GetDevice();
         D3D11_TEXTURE2D_DESC desc;
         ZeroMemory(&desc, sizeof(desc));
         desc.Width = size;
         desc.Height = size;
         desc.MipLevels = 0;
         desc.ArraySize = CUBE_FACES;
         desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
         desc.Usage = D3D11_USAGE_DEFAULT;
         desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
         desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
         desc.CPUAccessFlags = 0;
         desc.SampleDesc.Count = 1;
         desc.SampleDesc.Quality = 0;

         auto result = device.CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(&m_resource));
         if (!FAILED(result))
         {
            D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
            ZeroMemory(&rtvDesc, sizeof(rtvDesc));
            rtvDesc.Format = desc.Format;
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.ArraySize = 1;
            /* 0 단계 (원본) ~ log_2(size) 단계 (1 x 1) 까지 rtv 생성하기 **/

            for (unsigned int idx = 0; idx < CUBE_FACES; ++idx)
            {
               m_rtvs[idx].resize(static_cast<size_t>(m_maxMipLevels) + 1);
               rtvDesc.Texture2DArray.FirstArraySlice = idx;
               for (unsigned int mipLevel = 0; mipLevel <= m_maxMipLevels; ++mipLevel)
               {
                  rtvDesc.Texture2DArray.MipSlice = mipLevel;
                  result = device.CreateRenderTargetView(m_resource, &rtvDesc, &m_rtvs[idx][mipLevel]);
                  if (FAILED(result))
                  {
                     return false;
                  }
               }
            }

            if (!FAILED(result))
            {
               if (InitSRV(desc, true))
               {
                  RenderObject::ConfirmInit();
                  return true;
               }
            }
         }
      }

      return false;
   }

   bool DynamicCubemap::BindRenderTargetView(ID3D11DeviceContext& deviceContext, unsigned int faceIdx, unsigned int mipLevel)
   {
      if (RenderObject::IsBindable())
      {
         if (faceIdx < 6)
         {
            deviceContext.OMSetRenderTargets(1, &m_rtvs[faceIdx][mipLevel], nullptr);
            return true;
         }
      }

      return false;
   }

   void DynamicCubemap::UnbindRenderTargetView(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         deviceContext.OMSetRenderTargets(0, nullptr, nullptr);
      }
   }

   void DynamicCubemap::Clear(ID3D11DeviceContext& context, unsigned int faceIdx, unsigned int mipLevel, const Vector4& clearColor)
   {
      if (RenderObject::IsBindable())
      {
         context.ClearRenderTargetView(m_rtvs[faceIdx][mipLevel], clearColor.elements);
      }
   }

   void DynamicCubemap::ClearAll(ID3D11DeviceContext& context, const Vector4& clearColor)
   {
      for (unsigned int cubeFace = 0; cubeFace < 6; ++cubeFace)
      {
         for (unsigned int mipLevel = 0; mipLevel < m_rtvs[cubeFace].size(); ++mipLevel)
         {
            Clear(context, cubeFace, mipLevel, clearColor);
         }
      }
   }
}