#include "Rendering/GaussianBlurPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"

namespace Mile
{
   GaussianBlurPass::GaussianBlurPass(RendererDX11* renderer) :
      m_boundHdrBuffer(nullptr),
      m_outputHDRBuffer(nullptr),
      m_pingPongBuffer({ nullptr, }),
      m_params(nullptr),
      RenderingPass(renderer)
   {
   }

   GaussianBlurPass::~GaussianBlurPass()
   {
      for (size_t idx = 0; idx < m_pingPongBuffer.size(); ++idx)
      {
         SafeDelete(m_pingPongBuffer[idx]);
      }
      SafeDelete(m_params);
   }

   bool GaussianBlurPass::Init(unsigned int width, unsigned int height, DepthStencilBufferDX11* globalDepthStencilBuffer)
   {
      bool bValidParams = (width > 0) && (height > 0);
      if (bValidParams && RenderingPass::Init(TEXT("Contents/Shaders/GaussianBlur.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();

         m_params = new ConstantBufferDX11(renderer);
         if (!m_params->Init<BlurParameters>())
         {
            return false;
         }

         for (size_t idx = 0; idx < m_pingPongBuffer.size(); ++idx)
         {
            m_pingPongBuffer[idx] = new RenderTargetDX11(renderer);
            if (!m_pingPongBuffer[idx]->Init(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, globalDepthStencilBuffer))
            {
               return false;
            }
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

   bool GaussianBlurPass::Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* hdrBuffer)
   {
      bool bIsValidParams = hdrBuffer != nullptr;
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         bool bSuccess =
            hdrBuffer->BindAsShaderResource(deviceContext, 0, EShaderType::PixelShader) &&
            m_pingPongBuffer[1]->BindAsRenderTarget(deviceContext, true, false) &&
            m_params->Bind(deviceContext, 0, EShaderType::PixelShader);
         if (bSuccess)
         {
            m_boundHdrBuffer = hdrBuffer;
            m_outputHDRBuffer = m_pingPongBuffer[1];
            return true;
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void GaussianBlurPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         m_boundHdrBuffer->UnbindShaderResource(deviceContext);
         m_outputHDRBuffer->UnbindRenderTarget(deviceContext);
         m_params->Unbind(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   bool GaussianBlurPass::SwapBuffers(ID3D11DeviceContext& deviceContext, bool horizontal)
   {
      if (m_boundHdrBuffer != nullptr && m_outputHDRBuffer != nullptr)
      {
         m_boundHdrBuffer->UnbindShaderResource(deviceContext);
         m_outputHDRBuffer->UnbindRenderTarget(deviceContext);

         m_outputHDRBuffer = m_pingPongBuffer[horizontal];
         if (m_outputHDRBuffer->BindAsRenderTarget(deviceContext, true, false))
         {
            m_boundHdrBuffer = m_pingPongBuffer[!horizontal];
            if (m_boundHdrBuffer->BindAsShaderResource(deviceContext, 0, EShaderType::PixelShader))
            {
               return true;
            }
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void GaussianBlurPass::UpdateParameters(ID3D11DeviceContext& deviceContext, BlurParameters buffer)
   {
      if (m_params != nullptr)
      {
         m_params->Update(deviceContext, buffer);
      }
   }
}
