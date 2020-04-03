#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/Texture2dDX11.h"

namespace Mile
{
   RenderTargetDX11::RenderTargetDX11(RendererDX11* renderer) :
      m_renderer(renderer),
      m_depthStencilBuffer(nullptr),
      m_texture(nullptr),
      m_rtv(nullptr),
      m_width(0),
      m_height(0),
      m_clearColor{ 0.0f, 0.0f, 0.0f, 1.0f }
   {
   }

   RenderTargetDX11::~RenderTargetDX11()
   {
      SafeRelease(m_rtv);
      SafeDelete(m_texture);
   }

   bool RenderTargetDX11::Init(unsigned int width, unsigned int height, DXGI_FORMAT format, DepthStencilBufferDX11* depthStencilBuffer)
   {
      bool bIsValidRes = (width > 0 && height > 0);
      bool bIsReadyToInit = m_texture == nullptr && (bIsValidRes);
      if (bIsReadyToInit)
      {
         m_width = width;
         m_height = height;

         D3D11_TEXTURE2D_DESC texDesc;
         ZeroMemory(&texDesc, sizeof(texDesc));
         texDesc.Width = width;
         texDesc.Height = height;
         texDesc.MipLevels = 1;
         texDesc.ArraySize = 1;
         texDesc.Format = format;
         texDesc.SampleDesc.Count = 1;
         texDesc.SampleDesc.Quality = 0;
         texDesc.Usage = D3D11_USAGE_DEFAULT;
         texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
         texDesc.CPUAccessFlags = 0;
         texDesc.MiscFlags = 0;

         ID3D11Texture2D* texture = nullptr;
         auto device = m_renderer->GetDevice();
         auto result = device->CreateTexture2D(
            &texDesc,
            nullptr,
            &texture);
         if (!FAILED(result))
         {
            D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
            ZeroMemory(&rtvDesc, sizeof(rtvDesc));
            rtvDesc.Format = format;
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = 0;

            result = device->CreateRenderTargetView(
               texture,
               &rtvDesc,
               &m_rtv);
            if (!FAILED(result))
            {
               m_texture = new Texture2dDX11(m_renderer);
               if (m_texture->Init(texture))
               {
                  m_depthStencilBuffer = depthStencilBuffer;
                  return true;
               }
               else
               {
                  SafeDelete(m_texture);
               }
            }
            else
            {
               /* Faeild to Create RenderTargetView from D3D device. **/
            }
         }
         else
         {
            /* Failed to Create Texture2D from D3D Device. **/
         }
      }

      return false;
   }

   bool RenderTargetDX11::Init(ID3D11RenderTargetView* rtv, DepthStencilBufferDX11* depthStencilBuffer)
   {
      bool bIsReadyToInit = m_renderer != nullptr && rtv != nullptr && m_texture == nullptr;
      if (bIsReadyToInit)
      {
         m_rtv = rtv;
         ID3D11Resource* rtvResource = nullptr;
         m_rtv->GetResource(&rtvResource);
         if (rtvResource != nullptr)
         {
            ID3D11Texture2D* texture = static_cast<ID3D11Texture2D*>(rtvResource);
            D3D11_TEXTURE2D_DESC desc{ };
            texture->GetDesc(&desc);
            m_width = desc.Width;
            m_height = desc.Height;

            m_depthStencilBuffer = depthStencilBuffer;

            SafeRelease(texture);
            return true;
         }
         else
         {
            m_rtv = nullptr;
         }
      }

      return false;
   }

   bool RenderTargetDX11::BindAsRenderTarget(ID3D11DeviceContext& deviceContext, bool clearTarget)
   {
      bool bIsReadyToBind = m_rtv != nullptr && m_renderer != nullptr;
      if (bIsReadyToBind)
      {
         ID3D11DepthStencilView* dsv = nullptr;
         if (m_depthStencilBuffer != nullptr)
         {
            dsv = m_depthStencilBuffer->GetDSV();
         }

         if (clearTarget)
         {
            const float clearColor[4] = { m_clearColor.x, m_clearColor.y, m_clearColor.z, 1.0f };
            deviceContext.ClearRenderTargetView(m_rtv, clearColor);
            if (dsv != nullptr)
            {
               deviceContext.ClearDepthStencilView(dsv,
                  D3D11_CLEAR_DEPTH,
                  1.0f,
                  0);
            }
         }

         deviceContext.OMSetRenderTargets(1, &m_rtv, dsv);
         return true;
      }

      return false;
   }

   bool RenderTargetDX11::BindAsShaderResource(ID3D11DeviceContext& deviceContext, unsigned int startSlot, EShaderType shader)
   {
      if (m_texture != nullptr)
      {
         return m_texture->Bind(deviceContext, startSlot, shader);
      }

      return false;
   }

   void RenderTargetDX11::UnbindRenderTarget(ID3D11DeviceContext& deviceContext)
   {
      ID3D11RenderTargetView* nullRTV = nullptr;
      deviceContext.OMSetRenderTargets(1, &nullRTV, nullptr);
   }

   void RenderTargetDX11::UnbindShaderResource(ID3D11DeviceContext& deviceContext)
   {
      if (m_texture != nullptr)
      {
         m_texture->Unbind(deviceContext);
      }
   }

   void RenderTargetDX11::SetClearColor(const Vector4& color)
   {
      m_clearColor = color;
   }

   void RenderTargetDX11::ClearDepthStencil(ID3D11DeviceContext& deviceContext)
   {
      ID3D11DepthStencilView* dsv = nullptr;
      if (m_depthStencilBuffer != nullptr)
      {
         dsv = m_depthStencilBuffer->GetDSV();
         deviceContext.ClearDepthStencilView(
            dsv,
            D3D11_CLEAR_DEPTH,
            1.0f, 0);
      }
   }
}
