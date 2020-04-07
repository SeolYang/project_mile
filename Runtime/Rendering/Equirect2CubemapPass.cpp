#include "Rendering/Equirect2CubemapPass.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/DynamicCubemap.h"

namespace Mile
{
   Equirect2CubemapPass::Equirect2CubemapPass(RendererDX11* renderer) :
      m_cubemap(nullptr),
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

         RenderObject::ConfirmInit();
         return true;
      }

      return false;
   }

   bool Equirect2CubemapPass::Bind(ID3D11DeviceContext& deviceContext, Texture2dDX11* equirectangularMap)
   {
      if (RenderObject::IsBindable())
      {
      }

      return false;
   }

   void Equirect2CubemapPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
      }
   }
}