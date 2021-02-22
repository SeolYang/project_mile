#include "Rendering/SamplerDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   SamplerDX11::SamplerDX11(RendererDX11* renderer) :
      m_sampler(nullptr),
      RenderObject(renderer)
   {
   }

   SamplerDX11::~SamplerDX11()
   {
      SafeRelease(m_sampler);
   }

   bool SamplerDX11::Init(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressModeU, D3D11_TEXTURE_ADDRESS_MODE AddressModeV, D3D11_TEXTURE_ADDRESS_MODE AddressModeW, D3D11_COMPARISON_FUNC compFunc)
   {
      if (RenderObject::IsInitializable())
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

         RendererDX11* renderer = GetRenderer();
         auto& device = renderer->GetDevice();
         auto result = device.CreateSamplerState(&desc, &m_sampler);
         if (!FAILED(result))
         {
            RenderObject::ConfirmInit();
            return true;
         }
      }

      return false;
   }

   bool SamplerDX11::Bind(ID3D11DeviceContext& deviceContext, unsigned int bindSlot, EShaderType bindShader)
   {
      if (RenderObject::IsBindable())
      {
         switch (bindShader)
         {
         case Mile::EShaderType::VertexShader:
            deviceContext.VSSetSamplers(bindSlot, 1, &m_sampler);
            break;
         case Mile::EShaderType::HullShader:
            deviceContext.HSSetSamplers(bindSlot, 1, &m_sampler);
            break;
         case Mile::EShaderType::DomainShader:
            deviceContext.DSSetSamplers(bindSlot, 1, &m_sampler);
            break;
         case Mile::EShaderType::GeometryShader:
            deviceContext.GSSetSamplers(bindSlot, 1, &m_sampler);
            break;
         default:
         case Mile::EShaderType::PixelShader:
            deviceContext.PSSetSamplers(bindSlot, 1, &m_sampler);
            break;
         }
         return true;
      }

      return false;
   }

   void SamplerDX11::Unbind(ID3D11DeviceContext& deviceContext, unsigned int boundSlot, EShaderType bondShader)
   {
      if (RenderObject::IsBindable())
      {
         ID3D11SamplerState* nullSampler = nullptr;
         switch (bondShader)
         {
         case Mile::EShaderType::VertexShader:
            deviceContext.VSSetSamplers(boundSlot, 1, &nullSampler);
            break;
         case Mile::EShaderType::HullShader:
            deviceContext.HSSetSamplers(boundSlot, 1, &nullSampler);
            break;
         case Mile::EShaderType::DomainShader:
            deviceContext.DSSetSamplers(boundSlot, 1, &nullSampler);
            break;
         case Mile::EShaderType::GeometryShader:
            deviceContext.GSSetSamplers(boundSlot, 1, &nullSampler);
            break;
         default:
         case Mile::EShaderType::PixelShader:
            deviceContext.PSSetSamplers(boundSlot, 1, &nullSampler);
            break;
         }
      }
   }
}