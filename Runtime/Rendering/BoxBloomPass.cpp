#include "Rendering/BoxBloomPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"

namespace Mile
{
   BoxBloomPass::BoxBloomPass(RendererDX11* renderer) :
      m_depthStencilBuffer(nullptr),
      m_boundHdrBuffer(nullptr),
      m_outputHDRBuffer(nullptr),
      m_params(nullptr),
      RenderingPass(renderer)
   {
   }

   BoxBloomPass::~BoxBloomPass()
   {
      SafeDelete(m_depthStencilBuffer);
      SafeDelete(m_outputHDRBuffer);
      SafeDelete(m_params);
   }

   bool BoxBloomPass::Init(unsigned int width, unsigned int height)
   {
      bool bValidParams = (width > 0) && (height > 0);
      if (bValidParams && RenderingPass::Init(TEXT("Contents/Shaders/BoxBloom.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();

         m_params = new ConstantBufferDX11(renderer);
         if (!m_params->Init<BloomParameters>())
         {
            return false;
         }

         m_depthStencilBuffer = new DepthStencilBufferDX11(renderer);
         if (!m_depthStencilBuffer->Init(width, height, false))
         {
            return false;
         }

         m_outputHDRBuffer = new RenderTargetDX11(renderer);
         if (!m_outputHDRBuffer->Init(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, m_depthStencilBuffer))
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

   bool BoxBloomPass::Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* hdrBuffer)
   {
      bool bIsValidParams = hdrBuffer != nullptr;
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         bool bSuccess =
            hdrBuffer->BindAsShaderResource(deviceContext, 0, EShaderType::PixelShader) &&
            m_outputHDRBuffer->BindAsRenderTarget(deviceContext) &&
            m_params->Bind(deviceContext, 0, EShaderType::PixelShader);
         if (bSuccess)
         {
            m_boundHdrBuffer = hdrBuffer;
            return true;
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void BoxBloomPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         m_boundHdrBuffer->UnbindShaderResource(deviceContext);
         m_outputHDRBuffer->UnbindRenderTarget(deviceContext);
         m_params->Unbind(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   void BoxBloomPass::UpdateParameters(ID3D11DeviceContext& deviceContext, BloomParameters buffer)
   {
      if (m_params != nullptr)
      {
         m_params->Update(deviceContext, buffer);
      }
   }
}
