#include "Rendering/SamplerDX11.h"

namespace Mile
{
   SamplerDX11::SamplerDX11(RendererDX11* renderer) :
      m_sampler(nullptr),
      m_bIsInitialized(false),
      m_boundSlot(0),
      m_bIsBound(false),
      m_renderer(renderer)
   {
   }

   SamplerDX11::~SamplerDX11()
   {
      SafeRelease(m_sampler);
   }

   bool SamplerDX11::Init(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressModeU, D3D11_TEXTURE_ADDRESS_MODE AddressModeV, D3D11_TEXTURE_ADDRESS_MODE AddressModeW, D3D11_COMPARISON_FUNC compFunc)
   {
      if (m_bIsInitialized || m_renderer == nullptr)
      {
         return false;
      }
      bool bIsReadyToInit = (!m_bIsInitialized) && (m_renderer != nullptr);
      if (bIsReadyToInit)
      {
         D3D11_SAMPLER_DESC desc;
         ZeroMemory(&desc, sizeof(desc));
         desc.AddressU = AddressModeU;
         desc.AddressV = AddressModeV;
         desc.AddressW = AddressModeW;
         desc.Filter = filter;
         desc.ComparisonFunc = compFunc;
         desc.MipLODBias = 0.0f;
         desc.BorderColor[0] = 0.0f;
         desc.BorderColor[1] = 0.0f;
         desc.BorderColor[2] = 0.0f;
         desc.BorderColor[3] = 0.0f;
         desc.MinLOD = FLT_MIN;
         desc.MaxLOD = FLT_MAX;

         auto result = m_renderer->GetDevice()->CreateSamplerState(&desc, &m_sampler);
         if (!FAILED(result))
         {
            m_bIsInitialized = true;
            return true;
         }
      }

      return false;
   }

   bool SamplerDX11::Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot)
   {
      bool bIsReadyToBind = m_bIsInitialized && (m_renderer != nullptr);
      if (bIsReadyToBind)
      {
         deviceContext.PSSetSamplers(startSlot, 1, &m_sampler);
         m_boundSlot = startSlot;
         m_bIsBound = true;
         return true;
      }

      return false;
   }

   void SamplerDX11::Unbind(ID3D11DeviceContext& deviceContext)
   {
      bool bIsReadyToUnbind = m_bIsInitialized && m_bIsBound;
      if (bIsReadyToUnbind)
      {
         ID3D11SamplerState* nullSampler = nullptr;
         deviceContext.PSSetSamplers(m_boundSlot, 1, &nullSampler);
         m_boundSlot = 0;
         m_bIsBound = false;
      }
   }
}