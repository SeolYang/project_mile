#include "Rendering/GBuffer.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/BlendState.h"

constexpr size_t GBUFFER_RENDER_TARGET_NUM = 5;

namespace Mile
{
   GBuffer::GBuffer(RendererDX11* renderer) :
      m_renderer(renderer),
      m_depthStencilBuffer(nullptr),
      m_positionBuffer(nullptr),
      m_albedoBuffer(nullptr),
      m_emissiveAOBuffer(nullptr),
      m_normalBuffer(nullptr),
      m_metallicRoughnessBuffer(nullptr),
      m_blendState(nullptr)
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
      /* GBuffer가 Initialize 되어있지 않을때는 항상 모든 버퍼/상태 포인터가 nullptr을 가지기 때문에, 
      *  Position Buffer 에 대해서만 nullptr 체크를 한다.
      **/
      bool bIsInitialized = m_positionBuffer != nullptr;
      bool bIsReadyToInit = !bIsInitialized && m_renderer != nullptr;
      if (bIsReadyToInit)
      {

         m_positionBuffer = new RenderTargetDX11(m_renderer);
         m_albedoBuffer = new RenderTargetDX11(m_renderer);
         m_emissiveAOBuffer = new RenderTargetDX11(m_renderer);
         m_normalBuffer = new RenderTargetDX11(m_renderer);
         m_metallicRoughnessBuffer = new RenderTargetDX11(m_renderer);
         m_blendState = new BlendState(m_renderer);

         bool bBuffersInitialized =
            m_positionBuffer->Init(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT) &&
            m_albedoBuffer->Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM) &&
            m_emissiveAOBuffer->Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM) &&
            m_normalBuffer->Init(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT) &&
            m_metallicRoughnessBuffer->Init(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
         if (bBuffersInitialized && m_blendState->Init())
         {
            return true;
         }
         else
         {
            /* 앞에서 설명한 이유로인해 하나라도 실패하면 모든 객체를 삭제후 포인터 값을 nullptr로 유지한다. **/
            SafeDelete(m_positionBuffer);
            SafeDelete(m_albedoBuffer);
            SafeDelete(m_emissiveAOBuffer);
            SafeDelete(m_normalBuffer);
            SafeDelete(m_metallicRoughnessBuffer);
            SafeDelete(m_blendState);
         }
      }

      return false;
   }

   bool GBuffer::BindAsRenderTarget(ID3D11DeviceContext& deviceContext)
   {
      bool bIsInitialized = m_positionBuffer != nullptr;
      bool bIsReadyToBind = (m_renderer != nullptr) && bIsInitialized;
      if (bIsReadyToBind)
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

   bool GBuffer::BindAsShaderResource(ID3D11DeviceContext& deviceContext, unsigned int startSlot)
   {
      bool bIsInitialized = m_positionBuffer != nullptr;
      bool bIsReadyToBind = (m_renderer != nullptr) && bIsInitialized;
      if (bIsReadyToBind)
      {
         std::array<RenderTargetDX11*, GBUFFER_RENDER_TARGET_NUM> targets{
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

      return false;
   }

   void GBuffer::UnbindShaderResource(ID3D11DeviceContext& deviceContext)
   {
      bool bIsInitialized = m_positionBuffer != nullptr;
      if (bIsInitialized)
      {
         m_positionBuffer->UnbindShaderResource(deviceContext);
         m_albedoBuffer->UnbindShaderResource(deviceContext);
         m_emissiveAOBuffer->UnbindShaderResource(deviceContext);
         m_normalBuffer->UnbindShaderResource(deviceContext);
         m_metallicRoughnessBuffer->UnbindShaderResource(deviceContext);

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

   void GBuffer::UnbindRenderTarget(ID3D11DeviceContext& deviceContext)
   {
      bool bIsInitialized = m_positionBuffer != nullptr;
      if (bIsInitialized)
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