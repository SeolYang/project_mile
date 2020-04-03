#pragma once
#include "Rendering/ResourceDX11.h"

namespace Mile
{
   class MEAPI SamplerDX11
   {
   public:
      SamplerDX11(RendererDX11* renderer);
      ~SamplerDX11();

      bool Init(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressModeU, D3D11_TEXTURE_ADDRESS_MODE AddressModeV, D3D11_TEXTURE_ADDRESS_MODE AddressModeW, D3D11_COMPARISON_FUNC compFunc);
      bool Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot);
      void Unbind(ID3D11DeviceContext& deviceContext);

      FORCEINLINE bool IsInitialized() const { return m_bIsInitialized; }
      FORCEINLINE bool IsBound() const { return m_bIsBound; }

   private:
      RendererDX11* m_renderer;
      ID3D11SamplerState* m_sampler;

      bool m_bIsInitialized;
      bool m_bIsBound;
      unsigned int m_boundSlot;

   };
}