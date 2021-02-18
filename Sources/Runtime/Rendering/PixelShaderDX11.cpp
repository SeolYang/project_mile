#include "Rendering/PixelShaderDX11.h"
#include "Rendering/SamplerDX11.h"

namespace Mile
{
   PixelShaderDX11::PixelShaderDX11(RendererDX11* renderer) :
      m_shader(nullptr),
      ShaderDX11(renderer)
   {
   }

   PixelShaderDX11::~PixelShaderDX11()
   {
      SafeRelease(m_shader);
   }

   bool PixelShaderDX11::Init(const String& shaderPath)
   {
      if (RenderObject::IsInitializable())
      {
         if (Compile(shaderPath, EShaderType::PixelShader))
         {
            RendererDX11* renderer = GetRenderer();
            auto& device = renderer->GetDevice();
            auto result = device.CreatePixelShader(
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

   bool PixelShaderDX11::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         deviceContext.PSSetShader(m_shader,
            nullptr,
            0);

         return true;
      }

      return false;
   }

   void PixelShaderDX11::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         deviceContext.PSSetShader(nullptr, nullptr, 0);
      }
   }
}