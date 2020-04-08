#include "Rendering/Equirect2CubemapPass.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/DynamicCubemap.h"

namespace Mile
{
   Equirect2CubemapPass::Equirect2CubemapPass(RendererDX11* renderer) :
      m_cubemap(nullptr),
      m_boundEquirectMap(nullptr),
      RenderingPass(renderer)
   {
   }

   Equirect2CubemapPass::~Equirect2CubemapPass()
   {
      SafeDelete(m_cubemap);
   }

   bool Equirect2CubemapPass::Init(const String& shaderPath, unsigned int cubemapSize)
   {
      bool availableParams = shaderPath.length() > 0 && cubemapSize > 0;
      if (availableParams && RenderObject::IsInitializable())
      {
         m_cubemap = new DynamicCubemap(GetRenderer());
         if (!m_cubemap->Init(cubemapSize))
         {
            return false;
         }

         RenderObject::ConfirmInit();
         return true;
      }

      return false;
   }

   bool Equirect2CubemapPass::Bind(ID3D11DeviceContext& deviceContext, Texture2dDX11* equirectangularMap, unsigned int faceIndex)
   {
      bool bIsValidParams = equirectangularMap != nullptr && ((faceIndex > 0) && (faceIndex < 6));
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         if (equirectangularMap->Bind(deviceContext, 0, EShaderType::PixelShader))
         {
            m_boundEquirectMap = equirectangularMap;
            if (m_cubemap->BindAsRenderTarget(deviceContext, faceIndex))
            {
               return true;
            }
         }
      }

      return false;
   }

   void Equirect2CubemapPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      bool bIsValidUnbind = m_boundEquirectMap != nullptr;
      if (bIsValidUnbind && RenderingPass::IsBindable())
      {
         m_boundEquirectMap->Unbind(deviceContext);
         m_boundEquirectMap = nullptr;
         m_cubemap->UnbindAsRenderTarget(deviceContext);

         RenderingPass::Unbind(deviceContext);
      }
   }
}