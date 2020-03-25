#include "Rendering/GBuffer.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"

constexpr size_t NumOfRenderTargets = 5;

namespace Mile
{
   GBuffer::GBuffer(RendererDX11* renderer) :
      m_renderer(renderer),
      m_depthStencilBuffer(nullptr),
      m_positionBuffer(nullptr),
      m_albedoBuffer(nullptr),
      m_emissiveAOBuffer(nullptr),
      m_normalBuffer(nullptr),
      m_metallicRoughnessBuffer(nullptr)
   {
   }

   GBuffer::~GBuffer()
   {
      SafeDelete(m_positionBuffer);
      SafeDelete(m_albedoBuffer);
      SafeDelete(m_emissiveAOBuffer);
      SafeDelete(m_normalBuffer);
      SafeDelete(m_metallicRoughnessBuffer);
   }

   bool GBuffer::Init(unsigned int width, unsigned int height)
   {
      bool bIsInitialized = m_normalBuffer != nullptr || m_positionBuffer != nullptr;
      if (m_renderer == nullptr || bIsInitialized)
      {
         return false;
      }

      m_positionBuffer = new RenderTargetDX11(m_renderer);
      m_albedoBuffer = new RenderTargetDX11(m_renderer);
      m_emissiveAOBuffer = new RenderTargetDX11(m_renderer);
      m_normalBuffer = new RenderTargetDX11(m_renderer);
      m_metallicRoughnessBuffer = new RenderTargetDX11(m_renderer);

      if (!m_positionBuffer->Init(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT))
      {
         return false;
      }

      if (!m_albedoBuffer->Init(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT))
      {
         return false;
      }

      if (!m_emissiveAOBuffer->Init(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT))
      {
         return false;
      }

      if (!m_normalBuffer->Init(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT))
      {
         return false;
      }

      if (!m_metallicRoughnessBuffer->Init(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT))
      {
         return false;
      }

      return true;
   }

   bool GBuffer::BindAsRenderTarget(ID3D11DeviceContext& deviceContext)
   {
      bool bIsInitialized = m_normalBuffer != nullptr && m_positionBuffer != nullptr;
      if (m_renderer == nullptr || !bIsInitialized)
      {
         return false;
      }

      ID3D11DepthStencilView* dsv = nullptr;
      if (m_depthStencilBuffer != nullptr)
      {
         dsv = m_depthStencilBuffer->GetDSV();
         deviceContext.ClearDepthStencilView(dsv,
            D3D11_CLEAR_DEPTH,
            1.0f,
            0);
      }

      std::array<ID3D11RenderTargetView*, NumOfRenderTargets> targets{
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

      deviceContext.OMSetRenderTargets(NumOfRenderTargets, targets.data(), dsv);

      return true;
   }

   bool GBuffer::BindAsShaderResource(ID3D11DeviceContext& deviceContext, unsigned int startSlot)
   {
      bool bIsInitialized = m_normalBuffer != nullptr && m_positionBuffer != nullptr;
      if (m_renderer == nullptr || !bIsInitialized)
      {
         return false;
      }

      std::array<RenderTargetDX11*, NumOfRenderTargets> targets{
         m_positionBuffer,
         m_albedoBuffer,
         m_emissiveAOBuffer,
         m_normalBuffer,
         m_metallicRoughnessBuffer };

      for (unsigned int idx = 0; idx < targets.size(); ++idx)
      {
         targets[idx]->BindAsShaderResource(deviceContext, startSlot + idx, EShaderType::PixelShader);
      }

      return true;
   }

   void GBuffer::UnbindShaderResource(ID3D11DeviceContext& deviceContext)
   {
      bool bIsInitialized = m_normalBuffer != nullptr && m_positionBuffer != nullptr;
      if (bIsInitialized)
      {
         m_positionBuffer->UnbindShaderResource(deviceContext);
         m_albedoBuffer->UnbindShaderResource(deviceContext);
         m_emissiveAOBuffer->UnbindShaderResource(deviceContext);
         m_normalBuffer->UnbindShaderResource(deviceContext);
         m_metallicRoughnessBuffer->UnbindShaderResource(deviceContext);

         std::array<ID3D11RenderTargetView*, NumOfRenderTargets> targets{
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
         };

         deviceContext.OMSetRenderTargets(NumOfRenderTargets, targets.data(), nullptr);
      }
   }

   void GBuffer::UnbindRenderTarget(ID3D11DeviceContext& deviceContext)
   {
      bool bIsInitialized = m_normalBuffer != nullptr && m_positionBuffer != nullptr;
      if (bIsInitialized)
      {
         std::array<ID3D11RenderTargetView*, NumOfRenderTargets> targets{
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr
         };

         deviceContext.OMSetRenderTargets(NumOfRenderTargets, targets.data(), nullptr);
      }
   }
}