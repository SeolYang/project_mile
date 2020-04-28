#include "Rendering/SkyboxPass.h"
#include "Rendering/DepthStencilState.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/Texture2DBaseDX11.h"

namespace Mile
{
   SkyboxPass::SkyboxPass(RendererDX11* renderer) :
      m_depthStencilState(nullptr),
      m_boundEnvironmentMap(nullptr),
      m_transformBuffer(nullptr),
      RenderingPass(renderer)
   {
   }

   SkyboxPass::~SkyboxPass()
   {
      SafeDelete(m_transformBuffer);
      SafeDelete(m_depthStencilState);
   }

   bool SkyboxPass::Init()
   {
      if (RenderingPass::Init(TEXT("Contents/Shaders/SkyboxPass.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();
         m_depthStencilState = new DepthStencilState(renderer);
         D3D11_DEPTH_STENCIL_DESC depthStencilDesc = DepthStencilState::GetDefaultDesc();
         depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
         m_depthStencilState->SetDesc(depthStencilDesc);
         if (!m_depthStencilState->Init())
         {
            return false;
         }

         m_transformBuffer = new ConstantBufferDX11(renderer);
         if (!m_transformBuffer->Init<TransformConstantBuffer>())
         {
            return false;
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

   bool SkyboxPass::Bind(ID3D11DeviceContext& deviceContext, Texture2DBaseDX11* environmentMap)
   {
      bool bIsValidParams = environmentMap != nullptr;
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         if (!m_depthStencilState->Bind(deviceContext))
         {
            return false;
         }

         if (!m_transformBuffer->Bind(deviceContext, 0, EShaderType::VertexShader))
         {
            return false;
         }

         if (!environmentMap->Bind(deviceContext, 0, EShaderType::PixelShader))
         {
            return false;
         }
         m_boundEnvironmentMap = environmentMap;

         return true;
      }

      return false;
   }

   void SkyboxPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         m_boundEnvironmentMap->Unbind(deviceContext);
         m_transformBuffer->Unbind(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   void SkyboxPass::UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer)
   {
      if (m_transformBuffer != nullptr)
      {
         m_transformBuffer->Update(deviceContext, buffer);
      }
   }
}