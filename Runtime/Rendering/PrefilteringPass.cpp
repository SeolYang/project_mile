#include "Rendering/PrefilteringPass.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/DynamicCubemap.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/Viewport.h"
#include "Rendering/RasterizerState.h"
#include <cmath>

namespace Mile
{
   PrefilteringPass::PrefilteringPass(RendererDX11* renderer) :
      m_prefilteredEnvMap(nullptr),
      m_transformBuffer(nullptr),
      m_prefilteringParams(nullptr),
      m_boundEnvironmentMap(nullptr),
      RenderingPass(renderer)
   {
   }

   PrefilteringPass::~PrefilteringPass()
   {
      for (size_t idx = 0; idx < m_viewports.size(); ++idx)
      {
         SafeDelete(m_viewports[idx]);
      }

      SafeDelete(m_transformBuffer);
      SafeDelete(m_prefilteringParams);
      SafeDelete(m_prefilteredEnvMap);
   }

   bool PrefilteringPass::Init(unsigned int cubemapSize)
   {
      bool availableParams = cubemapSize > 0;
      if (availableParams &&
         RenderingPass::Init(TEXT("Contents/Shaders/Prefiltering.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();
         m_prefilteredEnvMap = new DynamicCubemap(renderer);
         if (!m_prefilteredEnvMap->Init(cubemapSize))
         {
            return false;
         }

         m_transformBuffer = new ConstantBufferDX11(renderer);
         if (!m_transformBuffer->Init<TransformConstantBuffer>())
         {
            return false;
         }

         m_prefilteringParams = new ConstantBufferDX11(renderer);
         if (!m_prefilteringParams->Init<PrefilteringParams>())
         {
            return false;
         }

         unsigned int maxMipLevels = m_prefilteredEnvMap->GetMaxMipLevels();
         m_viewports.resize(maxMipLevels);
         for (unsigned int mipLevel = 0; mipLevel < maxMipLevels; ++mipLevel)
         {
            float viewportSize = std::exp2(static_cast<float>(maxMipLevels - mipLevel));
            m_viewports[mipLevel] = new Viewport(renderer);
            m_viewports[mipLevel]->SetWidth(viewportSize);
            m_viewports[mipLevel]->SetHeight(viewportSize);
         }

         PixelShaderDX11* pixelShader = GetPixelShader();
         pixelShader->AddSampler(
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_TEXTURE_ADDRESS_CLAMP,
            D3D11_COMPARISON_ALWAYS);

         RenderObject::ConfirmInit();
         return true;
      }

      return false;
   }

   bool PrefilteringPass::Bind(ID3D11DeviceContext& deviceContext, Texture2DBaseDX11* environmentMap)
   {
      bool bValidParams = environmentMap != nullptr;
      if (RenderingPass::Bind(deviceContext))
      {
         bool bSuccess =
            environmentMap->Bind(deviceContext, 0, EShaderType::PixelShader) &&
            m_transformBuffer->Bind(deviceContext, 0, EShaderType::VertexShader) &&
            m_prefilteringParams->Bind(deviceContext, 0, EShaderType::PixelShader);
         if (bSuccess)
         {
            m_boundEnvironmentMap = environmentMap;
            return true;
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void PrefilteringPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         if (m_boundEnvironmentMap != nullptr)
         {
            m_boundEnvironmentMap->Unbind(deviceContext);
            m_boundEnvironmentMap = nullptr;
         }

         m_transformBuffer->Unbind(deviceContext);
         m_prefilteringParams->Unbind(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   void PrefilteringPass::UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer)
   {
      if (m_transformBuffer != nullptr)
      {
         m_transformBuffer->Update(deviceContext, buffer);
      }
   }

   void PrefilteringPass::UpdatePrefilteringParams(ID3D11DeviceContext& deviceContext, PrefilteringParams buffer)
   {
      if (m_prefilteringParams != nullptr)
      {
         m_prefilteringParams->Update(deviceContext, buffer);
      }
   }
}
