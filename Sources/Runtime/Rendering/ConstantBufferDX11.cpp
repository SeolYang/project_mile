#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   ConstantBufferDX11::ConstantBufferDX11(RendererDX11* renderer) :
      BufferDX11(renderer)
   {
   }

   bool ConstantBufferDX11::Init(unsigned int size)
   {
      if (size > 0 && RenderObject::IsInitializable())
      {
         D3D11_BUFFER_DESC desc;
         ZeroMemory(&desc, sizeof(desc));
         desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
         desc.ByteWidth = size;
         desc.StructureByteStride = 0;
         desc.Usage = D3D11_USAGE_DYNAMIC;
         desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
         desc.MiscFlags = 0;

         RendererDX11* renderer = GetRenderer();
         auto result = renderer->GetDevice().CreateBuffer(&desc, nullptr, reinterpret_cast<ID3D11Buffer**>(&m_resource));
         if (!FAILED(result))
         {
            m_desc = desc;
            ResourceDX11::ConfirmInit();
            return true;
         }
      }

      return false;
   }

   bool ConstantBufferDX11::Bind(ID3D11DeviceContext& deviceContext, unsigned int bindSlot, EShaderType bindShader)
   {
      if (RenderObject::IsBindable())
      {
         switch (bindShader)
         {
         case EShaderType::VertexShader:
            deviceContext.VSSetConstantBuffers(bindSlot, 1, reinterpret_cast<ID3D11Buffer**>(&m_resource));
            break;
         case EShaderType::HullShader:
            deviceContext.HSSetConstantBuffers(bindSlot, 1, reinterpret_cast<ID3D11Buffer**>(&m_resource));
            break;
         case EShaderType::DomainShader:
            deviceContext.DSSetConstantBuffers(bindSlot, 1, reinterpret_cast<ID3D11Buffer**>(&m_resource));
            break;
         case EShaderType::GeometryShader:
            deviceContext.GSSetConstantBuffers(bindSlot, 1, reinterpret_cast<ID3D11Buffer**>(&m_resource));
            break;
         case EShaderType::PixelShader:
            deviceContext.PSSetConstantBuffers(bindSlot, 1, reinterpret_cast<ID3D11Buffer**>(&m_resource));
            break;
         case EShaderType::ComputeShader:
            deviceContext.CSSetConstantBuffers(bindSlot, 1, reinterpret_cast<ID3D11Buffer**>(&m_resource));
            break;
         }

         return true;
      }

      return false;
   }

   void ConstantBufferDX11::Unbind(ID3D11DeviceContext& deviceContext, unsigned int boundSlot, EShaderType boundShader)
   {
      if (RenderObject::IsBindable())
      {
         ID3D11Buffer* nullBuffer = nullptr;
         switch (boundShader)
         {
         case EShaderType::VertexShader:
            deviceContext.VSSetConstantBuffers(boundSlot, 1, &nullBuffer);
            break;
         case EShaderType::HullShader:
            deviceContext.HSSetConstantBuffers(boundSlot, 1, &nullBuffer);
            break;
         case EShaderType::DomainShader:
            deviceContext.DSSetConstantBuffers(boundSlot, 1, &nullBuffer);
            break;
         case EShaderType::GeometryShader:
            deviceContext.GSSetConstantBuffers(boundSlot, 1, &nullBuffer);
            break;
         case EShaderType::PixelShader:
            deviceContext.PSSetConstantBuffers(boundSlot, 1, &nullBuffer);
            break;
         case EShaderType::ComputeShader:
            deviceContext.CSSetConstantBuffers(boundSlot, 1, &nullBuffer);
            break;
         }
      }
   }
}