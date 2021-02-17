#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   ConstantBufferDX11::ConstantBufferDX11(RendererDX11* renderer) :
      m_bIsBound(false),
      m_boundSlot(0),
      m_boundShader(EShaderType::PixelShader),
      BufferDX11(renderer)
   {
   }

   ConstantBufferDX11::~ConstantBufferDX11()
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
         auto result = renderer->GetDevice().CreateBuffer(&desc, nullptr, &m_buffer);
         if (!FAILED(result))
         {
            m_desc = desc;
            ResourceDX11::ConfirmInit();
            return true;
         }
      }

      return false;
   }

   void* ConstantBufferDX11::Map(ID3D11DeviceContext& deviceContext)
   {
      bool bIsReadyToMap = RenderObject::IsBindable() && (!IsMapped());
      if (bIsReadyToMap)
      {
         D3D11_MAPPED_SUBRESOURCE resource;
         auto result = deviceContext.Map(
            m_buffer,
            0, D3D11_MAP_WRITE_DISCARD, 0,
            &resource);

         if (!FAILED(result))
         {
            m_bIsMapped = true;
            return resource.pData;
         }
      }

      return nullptr;
   }

   bool ConstantBufferDX11::UnMap(ID3D11DeviceContext& deviceContext)
   {
      if (IsMapped())
      {
         deviceContext.Unmap(m_buffer, 0);
         m_bIsMapped = false;
         return true;
      }

      return false;
   }

   bool ConstantBufferDX11::Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot, EShaderType shaderType)
   {
      bool bIsReadyToBind = RenderObject::IsBindable() && !IsBound();
      if (bIsReadyToBind)
      {
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

         m_bIsBound = true;
         m_boundSlot = startSlot;
         m_boundShader = shaderType;
         return true;
      }

      return false;
   }

   void ConstantBufferDX11::Unbind(ID3D11DeviceContext& deviceContext)
   {
      bool IsReadyToUnbind = RenderObject::IsBindable() && IsBound();
      if (IsReadyToUnbind)
      {
         ID3D11Buffer* nullBuffer = nullptr;
         switch (m_boundShader)
         {
         case EShaderType::VertexShader:
            deviceContext.VSSetConstantBuffers(m_boundSlot, 1, &nullBuffer);
            break;
         case EShaderType::HullShader:
            deviceContext.HSSetConstantBuffers(m_boundSlot, 1, &nullBuffer);
            break;
         case EShaderType::DomainShader:
            deviceContext.DSSetConstantBuffers(m_boundSlot, 1, &nullBuffer);
            break;
         case EShaderType::GeometryShader:
            deviceContext.GSSetConstantBuffers(m_boundSlot, 1, &nullBuffer);
            break;
         case EShaderType::PixelShader:
            deviceContext.PSSetConstantBuffers(m_boundSlot, 1, &nullBuffer);
            break;
         default:
            return;
         }

         m_bIsBound = false;
      }
   }
}