#include "Rendering/InputLayoutDX11.h"
#include "Rendering/VertexShaderDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   InputLayoutDX11::InputLayoutDX11(RendererDX11* renderer) :
      m_inputLayout(nullptr),
      RenderObject(renderer)
   {
   }

   InputLayoutDX11::~InputLayoutDX11()
   {
      SafeRelease(m_inputLayout);
   }

   bool InputLayoutDX11::Init(InputLayoutElementList&& inputLayoutDescs, VertexShaderDX11* shader)
   {
      if (RenderObject::IsInitializable())
      {
         auto blob = shader->GetBlob();
         RendererDX11* renderer = GetRenderer();
         auto device = renderer->GetDevice();
         auto result = device->CreateInputLayout(
            inputLayoutDescs.data(),
            (unsigned int)inputLayoutDescs.size(),
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            &m_inputLayout);

         if (FAILED(result))
         {
            return false;
         }

         m_elementDescs = std::move(inputLayoutDescs);
         RenderObject::ConfirmInit();
         return true;
      }

      return false;
   }

   bool InputLayoutDX11::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         deviceContext.IASetInputLayout(m_inputLayout);
         return true;
      }

      return false;
   }
}