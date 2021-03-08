#pragma once
#include "Rendering/RenderObject.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   enum class ERenderResourceType
   {
      VertexBuffer,
      IndexBuffer,
      ConstantBuffer,
      StructuredBuffer,
      Texture1D,
      Texture2D,
      Texture3D,
      RenderTarget,
      DepthStencilBuffer,
      DynamicCubemap
   };

   class RendererDX11;
   class MEAPI ResourceDX11 : public RenderObject
   {
   public:
      ResourceDX11(RendererDX11* renderer) :
         m_resource(nullptr),
         m_srv(nullptr),
         m_uav(nullptr),
         m_bIsMapped(false),
         RenderObject(renderer)
      {
      }

      virtual ~ResourceDX11()
      {
         SafeRelease(m_srv);
         SafeRelease(m_uav);
         SafeRelease(m_resource);
      }

      ID3D11Resource* GetResource() const { return m_resource; }
      ID3D11ShaderResourceView* GetShaderResourceView() const { return m_srv; }
      ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return m_uav; }
      virtual ERenderResourceType GetResourceType() const = 0;

      FORCEINLINE bool IsMapped() const { return m_bIsMapped; }

      void* Map(ID3D11DeviceContext& deviceContext)
      {
         bool bIsReadyToMap = RenderObject::IsBindable() && (!IsMapped());
         if (bIsReadyToMap)
         {
            D3D11_MAPPED_SUBRESOURCE resource;
            auto result = deviceContext.Map(
               m_resource,
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

      bool UnMap(ID3D11DeviceContext& deviceContext)
      {
         if (IsMapped())
         {
            deviceContext.Unmap(m_resource, 0);
            m_bIsMapped = false;
            return true;
         }

         return false;
      }

      template <typename ResourceType>
      ResourceType* Map(ID3D11DeviceContext& deviceContext)
      {
         return reinterpret_cast<ResourceType*>(Map(deviceContext));
      }

      bool BindShaderResourceView(ID3D11DeviceContext& deviceContext, unsigned int bindSlot, EShaderType bindShader)
      {
         if (m_srv != nullptr)
         {
            switch (bindShader)
            {
            case EShaderType::VertexShader:
               deviceContext.VSSetShaderResources(bindSlot, 1, &m_srv);
               break;
            case EShaderType::HullShader:
               deviceContext.HSSetShaderResources(bindSlot, 1, &m_srv);
               break;
            case EShaderType::DomainShader:
               deviceContext.DSSetShaderResources(bindSlot, 1, &m_srv);
               break;
            case EShaderType::GeometryShader:
               deviceContext.GSSetShaderResources(bindSlot, 1, &m_srv);
               break;
            case EShaderType::PixelShader:
               deviceContext.PSSetShaderResources(bindSlot, 1, &m_srv);
               break;
            }

            return true;
         }

         return false;
      }

      void UnbindShaderResourceView(ID3D11DeviceContext& deviceContext, unsigned int boundSlot, EShaderType boundShader)
      {
         if (m_srv != nullptr)
         {
            ID3D11ShaderResourceView* nullSRV = nullptr;
            switch (boundShader)
            {
            case EShaderType::VertexShader:
               deviceContext.VSSetShaderResources(boundSlot, 1, &nullSRV);
               break;
            case EShaderType::GeometryShader:
               deviceContext.GSSetShaderResources(boundSlot, 1, &nullSRV);
               break;
            case EShaderType::DomainShader:
               deviceContext.DSSetShaderResources(boundSlot, 1, &nullSRV);
               break;
            case EShaderType::HullShader:
               deviceContext.HSSetShaderResources(boundSlot, 1, &nullSRV);
               break;
            case EShaderType::PixelShader:
               deviceContext.PSSetShaderResources(boundSlot, 1, &nullSRV);
               break;
            }
         }
      }

      bool BindUnorderedAccessView(ID3D11DeviceContext& deviceContext, unsigned int bindSlot)
      {
         if (m_uav != nullptr)
         {
            deviceContext.CSSetUnorderedAccessViews(bindSlot, 1, &m_uav, nullptr);
            return true;
         }

         return false;
      }

      void UnbindUnorderedAccessView(ID3D11DeviceContext& deviceContext, unsigned int boundSlot)
      {
         if (m_uav != nullptr)
         {
            ID3D11UnorderedAccessView* nullView = nullptr;
            deviceContext.CSSetUnorderedAccessViews(boundSlot, 1, &nullView, nullptr);
         }
      }

   protected:
      bool InitShaderResourceView(D3D11_SHADER_RESOURCE_VIEW_DESC desc)
      {
         if (m_srv == nullptr)
         {
            RendererDX11* renderer = GetRenderer();
            auto& device = renderer->GetDevice();
            auto result = device.CreateShaderResourceView(m_resource, &desc, &m_srv);
            if (!FAILED(result))
            {
               return true;
            }
         }

         return false;
      }

      bool InitUnorderedAccessView(D3D11_UNORDERED_ACCESS_VIEW_DESC desc)
      {
         if (m_uav == nullptr)
         {
            RendererDX11* renderer = GetRenderer();
            ID3D11Device& device = renderer->GetDevice();
            HRESULT result = device.CreateUnorderedAccessView(m_resource, &desc, &m_uav);
            if (!FAILED(result))
            {
               return true;
            }
         }

         return false;
      }

   protected:
      ID3D11Resource* m_resource;
      ID3D11ShaderResourceView* m_srv;
      ID3D11UnorderedAccessView* m_uav;
      bool m_bIsMapped;

   };
}