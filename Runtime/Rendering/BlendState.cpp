#include "Rendering/BlendState.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   BlendState::BlendState(class RendererDX11* renderer) :
      m_renderer(renderer),
      m_blendState(nullptr),
      m_bIsDirty(true),
      m_alphaToCoverageEnable(false),
      m_independentBlendEnable(false)
   {
   }

   BlendState::~BlendState()
   {
      SafeRelease(m_blendState);
   }

   bool BlendState::Init()
   {
      if (m_renderer == nullptr || m_blendState != nullptr)
      {
         return false;
      }

      D3D11_BLEND_DESC desc;
      ZeroMemory(&desc, sizeof(desc));
      desc.AlphaToCoverageEnable = m_alphaToCoverageEnable;
      desc.IndependentBlendEnable = m_independentBlendEnable;

      for (size_t idx = 0; idx < MAXIMUM_RENDER_TARGETS; ++idx)
      {
         desc.RenderTarget[idx] = m_blendDescs[idx].ToD3D11();
      }

      ID3D11Device* device = m_renderer->GetDevice();
      auto result = device->CreateBlendState(&desc, &m_blendState);
      if (FAILED(result))
      {
         m_bIsDirty = true;
         return false;
      }

      m_bIsDirty = false;
      return true;
   }

   bool BlendState::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (m_renderer == nullptr)
      {
         return false;
      }

      if (m_bIsDirty)
      {
         SafeRelease(m_blendState);
         if (!Init())
         {
            return false;
         }
      }

      deviceContext.OMSetBlendState(m_blendState, &m_blendFactor.x, m_sampleMask);
   }

   void BlendState::SetRenderTargetBlendState(RenderTargetBlendDesc desc, size_t renderTargetIndex /* = 0 */)
   {
      if (m_blendDescs[renderTargetIndex] != desc)
      {
         m_blendDescs[renderTargetIndex] = desc;
         m_bIsDirty = true;
      }
   }

   void BlendState::SetAlphaToCoverageEnable(bool bIsEnable)
   {
      if (bIsEnable != m_alphaToCoverageEnable)
      {
         m_alphaToCoverageEnable = bIsEnable;
         m_bIsDirty = true;
      }
   }

   void BlendState::SetIndependentBlendEnable(bool bIsEnable)
   {
      if (bIsEnable != m_independentBlendEnable)
      {
         m_independentBlendEnable = bIsEnable;
         m_bIsDirty = true;
      }
   }
}
