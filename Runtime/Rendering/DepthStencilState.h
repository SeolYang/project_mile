#pragma once
#include "Rendering/RenderObject.h"

namespace Mile
{
   class RendererDX11;
   class MEAPI DepthStencilState : public RenderObject
   {
   public:
      DepthStencilState(RendererDX11* renderer);
      ~DepthStencilState();

      bool Init();
      bool Bind(ID3D11DeviceContext& deviceContext);

      void SetDesc(D3D11_DEPTH_STENCIL_DESC desc) { m_desc = desc; m_bIsDirty = true; }
      D3D11_DEPTH_STENCIL_DESC GetDesc() const { return m_desc; }

      static D3D11_DEPTH_STENCIL_DESC GetDefaultDesc()
      {
         D3D11_DEPTH_STENCIL_DESC desc;
         ZeroMemory(&desc, sizeof(desc));
         desc.DepthEnable = true;
         desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
         desc.DepthFunc = D3D11_COMPARISON_LESS;
         desc.StencilEnable = false;
         desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
         desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
         desc.FrontFace.StencilFunc = desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
         desc.FrontFace.StencilDepthFailOp = desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
         desc.FrontFace.StencilPassOp = desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
         desc.FrontFace.StencilFailOp = desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
         return desc;
      }

   private:
      bool m_bIsDirty;
      D3D11_DEPTH_STENCIL_DESC m_desc;
      ID3D11DepthStencilState* m_depthStencilState;

   };
}