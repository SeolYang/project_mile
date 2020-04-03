#pragma once
#include "Rendering/ResourceDX11.h"

namespace Mile
{
   class VertexShaderDX11;
   class MEAPI InputLayoutDX11
   {
      using InputLayoutElementList = std::vector<D3D11_INPUT_ELEMENT_DESC>;
   public:
      InputLayoutDX11(RendererDX11* renderer);
      ~InputLayoutDX11();

      bool Init(InputLayoutElementList&& intputLayoutDescs, VertexShaderDX11* shader);
      bool Bind(ID3D11DeviceContext& deviceContext);

      InputLayoutElementList GetElementDescs() const { return m_elementDescs; }

   private:
      RendererDX11* m_renderer;
      bool m_bIsInitialized;
      ID3D11InputLayout* m_inputLayout;
      InputLayoutElementList m_elementDescs;

   };
}
