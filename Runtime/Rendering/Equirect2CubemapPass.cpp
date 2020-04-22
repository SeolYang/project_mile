#include "Rendering/Equirect2CubemapPass.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/DynamicCubemap.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/Viewport.h"
#include "Rendering/RasterizerState.h"

namespace Mile
{
   Equirect2CubemapPass::Equirect2CubemapPass(RendererDX11* renderer) :
      m_viewport(nullptr),
      m_cubemap(nullptr),
      m_transformBuffer(nullptr),
      m_boundEquirectMap(nullptr),
      RenderingPass(renderer)
   {
   }

   Equirect2CubemapPass::~Equirect2CubemapPass()
   {
      SafeDelete(m_viewport);
      SafeDelete(m_transformBuffer);
      SafeDelete(m_cubemap);
   }

   bool Equirect2CubemapPass::Init(unsigned int cubemapSize)
   {
      bool availableParams = cubemapSize > 0;
      if (availableParams &&
         RenderingPass::Init(TEXT("Contents/Shaders/Equirectangular2Cube.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();
         m_cubemap = new DynamicCubemap(renderer);
         if (!m_cubemap->Init(cubemapSize))
         {
            return false;
         }

         m_transformBuffer = new ConstantBufferDX11(renderer);
         if (!m_transformBuffer->Init(sizeof(TransformConstantBuffer)))
         {
            return false;
         }

         m_viewport = new Viewport(renderer);
         m_viewport->SetWidth(static_cast<float>(cubemapSize));
         m_viewport->SetHeight(static_cast<float>(cubemapSize));

         PixelShaderDX11* pixelShader = GetPixelShader();
         pixelShader->AddSampler(
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_TEXTURE_ADDRESS_WRAP,
            D3D11_COMPARISON_ALWAYS);

         RenderObject::ConfirmInit();
         return true;
      }

      return false;
   }

   bool Equirect2CubemapPass::Bind(ID3D11DeviceContext& deviceContext, Texture2dDX11* equirectangularMap)
   {
      if (RenderingPass::Bind(deviceContext))
      {
         if (equirectangularMap != nullptr)
         {
            if (!equirectangularMap->Bind(deviceContext, 0, EShaderType::PixelShader))
            {
               return false;
            }
         }

         m_boundEquirectMap = equirectangularMap;
         if (!m_viewport->Bind(deviceContext))
         {
            return false;
         }

         if (!m_transformBuffer->Bind(deviceContext, 0, EShaderType::VertexShader))
         {
            return false;
         }

         return true;
      }

      return false;
   }

   void Equirect2CubemapPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         if (m_boundEquirectMap != nullptr)
         {
            m_boundEquirectMap->Unbind(deviceContext);
            m_boundEquirectMap = nullptr;
         }

         m_transformBuffer->Unbind(deviceContext);
         m_cubemap->UnbindAsRenderTarget(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   void Equirect2CubemapPass::UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer)
   {
      if (m_transformBuffer != nullptr)
      {
         auto mappedBuffer = reinterpret_cast<TransformConstantBuffer*>(m_transformBuffer->Map(deviceContext));
         if (mappedBuffer != nullptr)
         {
            (*mappedBuffer) = buffer;
         }
         m_transformBuffer->UnMap(deviceContext);
      }
   }
}
