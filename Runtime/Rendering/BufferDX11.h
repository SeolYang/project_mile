#pragma once
#include "Rendering/ResourceDX11.h"

namespace Mile
{
   class MEAPI BufferDX11 : public ResourceDX11
   {
   public:
      BufferDX11(RendererDX11* renderer);
      ~BufferDX11();

      virtual ID3D11Resource* GetResource() const override { return m_buffer; }
      D3D11_BUFFER_DESC GetDesc() const { return m_desc; }

      virtual void* Map(ID3D11DeviceContext& deviceContext) { return nullptr; }
      virtual bool UnMap(ID3D11DeviceContext& deviceContext) { return false; }

      FORCEINLINE bool IsMapped() const { return m_bIsMapped; }

   protected:
      ID3D11Buffer*     m_buffer;
      D3D11_BUFFER_DESC m_desc;
      bool              m_bIsMapped;

   };
}