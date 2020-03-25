#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   bool ConstantBufferDX11::Init(unsigned int size)
   {
      if (m_bIsInitialized
         || (m_renderer == nullptr)
         || (size == 0))
      {
         return false;
      }

      D3D11_BUFFER_DESC desc;
      ZeroMemory(&desc, sizeof(desc));
      desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      desc.ByteWidth = size;
      desc.StructureByteStride = 0;
      desc.Usage = D3D11_USAGE_DYNAMIC;
      desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      desc.MiscFlags = 0;

      auto result = m_renderer->GetDevice()->CreateBuffer(&desc, nullptr, &m_buffer);
      if (FAILED(result))
      {
         return false;
      }

      m_desc = desc;
      m_bIsInitialized = true;
      return true;
   }

   void* ConstantBufferDX11::Map(ID3D11DeviceContext& deviceContext)
   {
      if (!m_bIsInitialized || m_renderer == nullptr || m_bIsMapped)
      {
         return nullptr;
      }

      D3D11_MAPPED_SUBRESOURCE resource;
      auto result = deviceContext.Map(
         m_buffer,
         0, D3D11_MAP_WRITE_DISCARD, 0,
         &resource);

      if (FAILED(result))
      {
         return nullptr;
      }

      m_bIsMapped = true;
      return resource.pData;
   }

   bool ConstantBufferDX11::UnMap(ID3D11DeviceContext& deviceContext)
   {
      if (!m_bIsInitialized || m_renderer == nullptr || !m_bIsMapped)
      {
         return false;
      }

      deviceContext.Unmap(m_buffer, 0);
      m_bIsMapped = false;
      return true;
   }

   bool ConstantBufferDX11::Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot, EShaderType shaderType)
   {
      if (!m_bIsInitialized || m_renderer == nullptr || m_bIsBinded)
      {
         return false;
      }

      switch (shaderType)
      {
      case EShaderType::VertexShader:
         deviceContext.VSSetConstantBuffers(startSlot, 1, &m_buffer);
         break;
      case EShaderType::HullShader:
         deviceContext.HSSetConstantBuffers(startSlot, 1, &m_buffer);
         break;
      case EShaderType::DomainShader:
         deviceContext.DSSetConstantBuffers(startSlot, 1, &m_buffer);
         break;
      case EShaderType::GeometryShader:
         deviceContext.GSSetConstantBuffers(startSlot, 1, &m_buffer);
         break;
      case EShaderType::PixelShader:
         deviceContext.PSSetConstantBuffers(startSlot, 1, &m_buffer);
         break;
      default:
         return false;
      }

      m_bindedSlot = startSlot;
      m_bindedShader = shaderType;
      m_bIsBinded = true;
      return true;
   }

   void ConstantBufferDX11::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (!m_bIsInitialized || m_renderer == nullptr || !m_bIsBinded)
      {
         return;
      }

      ID3D11Buffer* nullBuffer = nullptr;

      switch (m_bindedShader)
      {
      case EShaderType::VertexShader:
         deviceContext.VSSetConstantBuffers(m_bindedSlot, 1, &nullBuffer);
         break;
      case EShaderType::HullShader:
         deviceContext.HSSetConstantBuffers(m_bindedSlot, 1, &nullBuffer);
         break;
      case EShaderType::DomainShader:
         deviceContext.DSSetConstantBuffers(m_bindedSlot, 1, &nullBuffer);
         break;
      case EShaderType::GeometryShader:
         deviceContext.GSSetConstantBuffers(m_bindedSlot, 1, &nullBuffer);
         break;
      case EShaderType::PixelShader:
         deviceContext.PSSetConstantBuffers(m_bindedSlot, 1, &nullBuffer);
         break;
      default:
         return;
      }

      m_bIsBinded = false;
   }
}