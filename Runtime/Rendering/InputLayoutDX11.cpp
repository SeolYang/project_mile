#include "Rendering/InputLayoutDX11.h"
#include "Rendering/VertexShaderDX11.h"

namespace Mile
{
   InputLayoutDX11::InputLayoutDX11(RendererDX11* renderer) :
      m_renderer(renderer),
      m_inputLayout(nullptr),
      m_bIsInitialized(false)
   {
   }

   InputLayoutDX11::~InputLayoutDX11()
   {
      SafeRelease(m_inputLayout);
   }

   bool InputLayoutDX11::Init(InputLayoutElementList&& inputLayoutDescs, VertexShaderDX11* shader)
   {
      if (m_bIsInitialized || m_renderer == nullptr)
      {
         return false;
      }

      auto blob = shader->GetBlob();
      auto result = m_renderer->GetDevice()->CreateInputLayout(inputLayoutDescs.data(),
         (unsigned int)inputLayoutDescs.size(),
         blob->GetBufferPointer(),
         blob->GetBufferSize(),
         &m_inputLayout);

      if (FAILED(result))
      {
         return false;
      }

      m_elementDescs = std::move(inputLayoutDescs);
      m_bIsInitialized = true;
      return true;
   }

   bool InputLayoutDX11::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (!m_bIsInitialized || m_renderer == nullptr)
      {
         return false;
      }

      deviceContext.IASetInputLayout(m_inputLayout);
      return true;
   }
}