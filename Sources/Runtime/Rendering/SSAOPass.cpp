#include "Rendering/SSAOPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/GBuffer.h"

namespace Mile
{
   SSAOPass::SSAOPass(RendererDX11* renderer) :
      m_boundGBuffer(nullptr),
      m_boundNoise(nullptr),
      m_outputBuffer(nullptr),
      m_params(nullptr),
      m_variableParams(nullptr),
      RenderingPass(renderer)
   {
   }

   SSAOPass::~SSAOPass()
   {
      SafeDelete(m_outputBuffer);
      SafeDelete(m_variableParams);
      SafeDelete(m_params);
   }

   bool SSAOPass::Init(unsigned int width, unsigned int height, DepthStencilBufferDX11* globalDepthStencilBuffer)
   {
      bool bValidParams = (width > 0) && (height > 0);
      if (bValidParams && RenderingPass::Init(TEXT("Contents/Shaders/SSAO.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();

         m_params = new ConstantBufferDX11(renderer);
         if (!m_params->Init<SSAOParams>())
         {
            return false;
         }

         m_variableParams = new ConstantBufferDX11(renderer);
         if (!m_variableParams->Init<SSAOVariableParams>())
         {
            return false;
         }

         m_outputBuffer = new RenderTargetDX11(renderer);
         if (!m_outputBuffer->Init(width, height, DXGI_FORMAT_R32_FLOAT, globalDepthStencilBuffer))
         {
            return false;
         }

         PixelShaderDX11* pixelShader = GetPixelShader();
         pixelShader->AddSampler(
            D3D11_FILTER_MIN_MAG_MIP_POINT,
            D3D11_TEXTURE_ADDRESS_CLAMP,
            D3D11_COMPARISON_ALWAYS);

         pixelShader->AddSampler(
            D3D11_FILTER_MIN_MAG_MIP_POINT,
            D3D11_TEXTURE_ADDRESS_WRAP,
            D3D11_COMPARISON_ALWAYS);

         pixelShader->AddSampler(
            D3D11_FILTER_MIN_MAG_MIP_POINT,
            D3D11_TEXTURE_ADDRESS_CLAMP,
            D3D11_COMPARISON_ALWAYS);

         RenderObject::ConfirmInit();
         return true;
      }

      return false;
   }

   bool SSAOPass::Bind(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer, Texture2dDX11* noiseTexture)
   {
      bool bIsValidParams = noiseTexture != nullptr && gBuffer != nullptr;
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         bool bSuccess =
            gBuffer->BindAsShaderResource(deviceContext, 0) &&
            noiseTexture->Bind(deviceContext, 5, EShaderType::PixelShader) &&
            m_outputBuffer->BindAsRenderTarget(deviceContext, true, false) &&
            m_params->Bind(deviceContext, 0, EShaderType::PixelShader) &&
            m_variableParams->Bind(deviceContext, 1, EShaderType::PixelShader);
         if (bSuccess)
         {
            m_boundGBuffer = gBuffer;
            m_boundNoise = noiseTexture;
            return true;
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void SSAOPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         if (m_boundGBuffer != nullptr)
         {
            m_boundGBuffer->UnbindShaderResource(deviceContext);
            m_boundGBuffer = nullptr;
         }

         if (m_boundNoise != nullptr)
         {
            m_boundNoise->Unbind(deviceContext);
            m_boundNoise = nullptr;
         }

         m_outputBuffer->UnbindRenderTarget(deviceContext);
         m_params->Unbind(deviceContext);
         m_variableParams->Unbind(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   void SSAOPass::UpdateParameters(ID3D11DeviceContext& deviceContext, SSAOParams buffer)
   {
      if (m_params != nullptr)
      {
         m_params->Update(deviceContext, buffer);
      }
   }
   void SSAOPass::UpdateVariableParams(ID3D11DeviceContext& deviceContext, SSAOVariableParams buffer)
   {
      if (m_variableParams != nullptr)
      {
         m_variableParams->Update(deviceContext, buffer);
      }
   }
}
