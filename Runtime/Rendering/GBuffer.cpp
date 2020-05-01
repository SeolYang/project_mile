#include "Rendering/GBuffer.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/BlendState.h"

constexpr unsigned int GBUFFER_RENDER_TARGET_NUM = 5;

namespace Mile
{
   GBuffer::GBuffer(RendererDX11* renderer) :
      m_depthStencilBuffer(nullptr),
      m_positionBuffer(nullptr),
      m_albedoBuffer(nullptr),
      m_emissiveAOBuffer(nullptr),
      m_normalBuffer(nullptr),
      m_metallicRoughnessBuffer(nullptr),
      m_blendState(nullptr),
      m_bBoundDepthAsShaderResource(false),
      RenderObject(renderer)
   {
   }

   GBuffer::~GBuffer()
   {
      SafeDelete(m_positionBuffer);
      SafeDelete(m_albedoBuffer);
      SafeDelete(m_emissiveAOBuffer);
      SafeDelete(m_normalBuffer);
      SafeDelete(m_metallicRoughnessBuffer);
      SafeDelete(m_blendState);
   }

   bool GBuffer::Init(unsigned int width, unsigned int height)
   {
      if (RenderObject::IsInitializable())
      {
         RendererDX11* renderer = GetRenderer();
         m_positionBuffer = new RenderTargetDX11(renderer);
         m_albedoBuffer = new RenderTargetDX11(renderer);
         m_emissiveAOBuffer = new RenderTargetDX11(renderer);
         m_normalBuffer = new RenderTargetDX11(renderer);
         m_metallicRoughnessBuffer = new RenderTargetDX11(renderer);
         m_blendState = new BlendState(renderer);

         bool bBuffersInitialized =
            m_positionBuffer->Init(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT) &&
            m_albedoBuffer->Init(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT) &&
            m_emissiveAOBuffer->Init(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT) &&
            m_normalBuffer->Init(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT) &&
            m_metallicRoughnessBuffer->Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
         if (bBuffersInitialized && m_blendState->Init())
         {
            RenderObject::ConfirmInit();
            return true;
         }
      }

      return false;
   }

   bool GBuffer::BindAsRenderTarget(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         ID3D11DepthStencilView* dsv = nullptr;
         if (m_depthStencilBuffer != nullptr)
         {
            dsv = m_depthStencilBuffer->GetDSV();
            deviceContext.ClearDepthStencilView(dsv,
               D3D11_CLEAR_DEPTH,
               1.0f,
               0);
         }

         std::array<ID3D11RenderTargetView*, GBUFFER_RENDER_TARGET_NUM> targets{
            m_positionBuffer->GetRTV(),
            m_albedoBuffer->GetRTV(),
            m_emissiveAOBuffer->GetRTV(),
            m_normalBuffer->GetRTV(),
            m_metallicRoughnessBuffer->GetRTV()
         };

         const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
         for (auto rtv : targets)
         {
            deviceContext.ClearRenderTargetView(rtv, clearColor);
         }

         deviceContext.OMSetRenderTargets(GBUFFER_RENDER_TARGET_NUM, targets.data(), dsv);

         if (m_blendState != nullptr)
         {
            if (m_blendState->Bind(deviceContext))
            {
               return true;
            }
         }
      }
      
      return false;
   }

   bool GBuffer::BindAsShaderResource(ID3D11DeviceContext& deviceContext, unsigned int startSlot, bool bBindDepthStencil)
   {
      if (RenderObject::IsBindable())
      {
         std::array<RenderTargetDX11*, GBUFFER_RENDER_TARGET_NUM> targets{
         m_positionBuffer,
         m_albedoBuffer,
         m_emissiveAOBuffer,
         m_normalBuffer,
         m_metallicRoughnessBuffer };

         for (unsigned int idx = 0; idx < GBUFFER_RENDER_TARGET_NUM; ++idx)
         {
            if (!targets[idx]->BindAsShaderResource(deviceContext, startSlot + idx, EShaderType::PixelShader))
            {
               return false;
            }
         }

         if (bBindDepthStencil)
         {
            ID3D11ShaderResourceView* depthSRV = m_depthStencilBuffer->GetSRV();
            deviceContext.PSSetShaderResources(startSlot + GBUFFER_RENDER_TARGET_NUM, 1, &depthSRV);
            m_bBoundDepthAsShaderResource = bBindDepthStencil;
         }

         return true;
      }

      return false;
   }

   void GBuffer::UnbindShaderResource(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         m_positionBuffer->UnbindShaderResource(deviceContext);
         m_albedoBuffer->UnbindShaderResource(deviceContext);
         m_emissiveAOBuffer->UnbindShaderResource(deviceContext);
         m_normalBuffer->UnbindShaderResource(deviceContext);
         m_metallicRoughnessBuffer->UnbindShaderResource(deviceContext);

         if (m_bBoundDepthAsShaderResource)
         {
            ID3D11ShaderResourceView* nullSRV = nullptr;
            Texture2dDX11* positionTexture = m_positionBuffer->GetTexture();
            unsigned int startSlot = positionTexture->GetBoundSlot();
            deviceContext.PSSetShaderResources(startSlot + GBUFFER_RENDER_TARGET_NUM, 1, &nullSRV);
            m_bBoundDepthAsShaderResource = false;
         }
      }
   }

   void GBuffer::UnbindRenderTarget(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         std::array<ID3D11RenderTargetView*, GBUFFER_RENDER_TARGET_NUM> targets{
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
         };

         deviceContext.OMSetRenderTargets(GBUFFER_RENDER_TARGET_NUM, targets.data(), nullptr);
      }
   }
}