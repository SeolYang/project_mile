#pragma once
#include "Rendering/RenderObject.h"

namespace Mile
{
   class VertexShaderDX11;
   class MEAPI InputLayoutDX11 : public RenderObject
   {
      using InputLayoutElementList = std::vector<D3D11_INPUT_ELEMENT_DESC>;
   public:
      InputLayoutDX11(RendererDX11* renderer);
      virtual ~InputLayoutDX11();

      bool Init(InputLayoutElementList&& intputLayoutDescs, VertexShaderDX11* shader);
      bool Bind(ID3D11DeviceContext& deviceContext);

      InputLayoutElementList GetElementDescs() const { return m_elementDescs; }

   private:
      ID3D11InputLayout* m_inputLayout;
      InputLayoutElementList m_elementDescs;

   };
}
