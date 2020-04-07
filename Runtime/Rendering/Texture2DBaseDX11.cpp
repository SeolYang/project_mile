#include "Rendering/Texture2DBaseDX11.h"

namespace Mile
{
   Texture2DBaseDX11::Texture2DBaseDX11(RendererDX11* renderer) :
      m_texture(nullptr),
      m_srv(nullptr),
      m_mipLevels(1),
      m_bIsBoundAsShaderResource(false),
      m_boundSlot(0),
      m_boundShader(EShaderType::PixelShader),
      m_width(0),
      m_height(0),
      ResourceDX11(renderer)
   {
   }

   Texture2DBaseDX11::~Texture2DBaseDX11()
   {
      SafeRelease(m_srv);
      SafeRelease(m_texture);
   }

   bool Texture2DBaseDX11::Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot, EShaderType shader)
   {
      if (RenderObject::IsBindable() && !IsBoundAsShaderResource())
      {
         switch (shader)
         {
         case EShaderType::VertexShader:
            deviceContext.VSSetShaderResources(startSlot, 1, &m_srv);
            break;
         case EShaderType::GeometryShader:
            deviceContext.GSSetShaderResources(startSlot, 1, &m_srv);
         case EShaderType::DomainShader:
            deviceContext.DSSetShaderResources(startSlot, 1, &m_srv);
            break;
         case EShaderType::HullShader:
            deviceContext.HSSetShaderResources(startSlot, 1, &m_srv);
            break;
         case EShaderType::PixelShader:
            deviceContext.PSSetShaderResources(startSlot, 1, &m_srv);
            break;
         }

         m_boundSlot = startSlot;
         m_boundShader = shader;

         m_bIsBoundAsShaderResource = true;
         return true;
      }

      return false;
   }

   void Texture2DBaseDX11::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable() && IsBoundAsShaderResource())
      {
         unsigned int boundSlot = GetBoundSlot();
         ID3D11ShaderResourceView* nullSRV = nullptr;
         switch (GetBoundShaderType())
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

         m_bIsBoundAsShaderResource = false;
      }
   }

}
