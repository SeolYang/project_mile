#include "Rendering/ComputeShaderDX11.h"

namespace Mile
{
   ComputeShaderDX11::ComputeShaderDX11(RendererDX11* renderer) :
      m_shader(nullptr),
      ShaderDX11(renderer)
   {
   }

   ComputeShaderDX11::~ComputeShaderDX11()
   {
      SafeRelease(m_shader);
   }

   bool ComputeShaderDX11::Init(const String& shaderPath)
   {
      if (m_shader != nullptr)
      {
         if (Compile(shaderPath, EShaderType::ComputeShader))
         {
            RendererDX11* renderer = GetRenderer();
            auto& device = renderer->GetDevice();
            HRESULT result = device.CreateComputeShader(
               m_blob->GetBufferPointer(),
               m_blob->GetBufferSize(),
               nullptr,
               &m_shader);

            if (!FAILED(result))
            {
               RenderObject::ConfirmInit();
               return true;
            }
         }
      }

      return false;
   }

   bool ComputeShaderDX11::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         deviceContext.CSSetShader(m_shader,
            nullptr,
            0);

         return true;
      }

      return false;
   }

   void ComputeShaderDX11::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         deviceContext.CSSetShader(nullptr, nullptr, 0);
      }
   }
}