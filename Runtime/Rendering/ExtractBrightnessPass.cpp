#include "Rendering/ExtractBrightnessPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/GBuffer.h"

namespace Mile
{
   ExtractBrightnessPass::ExtractBrightnessPass(RendererDX11* renderer) :
      m_depthStencilBuffer(nullptr),
      m_boundGBuffer(nullptr),
      m_boundHdrBuffer(nullptr),
      m_outputHDRBuffer(nullptr),
      m_params(nullptr),
      RenderingPass(renderer)
   {
   }

   ExtractBrightnessPass::~ExtractBrightnessPass()
   {
      SafeDelete(m_depthStencilBuffer);
      SafeDelete(m_outputHDRBuffer);
      SafeDelete(m_params);
   }

   bool ExtractBrightnessPass::Init(unsigned int width, unsigned int height)
   {
      bool bValidParams = (width > 0) && (height > 0);
      if (bValidParams && RenderingPass::Init(TEXT("Contents/Shaders/ExtractBrightness.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();

         m_params = new ConstantBufferDX11(renderer);
         if (!m_params->Init<ExtractParams>())
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

   bool ExtractBrightnessPass::Bind(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer, RenderTargetDX11* hdrBuffer)
   {
      bool bIsValidParams = hdrBuffer != nullptr && gBuffer != nullptr;
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         bool bSuccess =
            gBuffer->BindAsShaderResource(deviceContext, 0, true) &&
            hdrBuffer->BindAsShaderResource(deviceContext, 6, EShaderType::PixelShader) &&
            m_outputHDRBuffer->BindAsRenderTarget(deviceContext) &&
            m_params->Bind(deviceContext, 0, EShaderType::PixelShader);
         if (bSuccess)
         {
            m_boundGBuffer = gBuffer;
            m_boundHdrBuffer = hdrBuffer;
            return true;
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void ExtractBrightnessPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         if (m_boundGBuffer != nullptr)
         {
            m_boundGBuffer->UnbindShaderResource(deviceContext);
            m_boundGBuffer = nullptr;
         }

         if (m_boundHdrBuffer != nullptr)
         {
            m_boundHdrBuffer->UnbindShaderResource(deviceContext);
            m_boundHdrBuffer = nullptr;
         }

         m_outputHDRBuffer->UnbindRenderTarget(deviceContext);
         m_params->Unbind(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   void ExtractBrightnessPass::UpdateParameters(ID3D11DeviceContext& deviceContext, ExtractParams buffer)
   {
      if (m_params != nullptr)
      {
         m_params->Update(deviceContext, buffer);
      }
   }
}
