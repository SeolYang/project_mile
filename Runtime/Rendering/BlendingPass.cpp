#include "Rendering/BlendingPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"

namespace Mile
{
   BlendingPass::BlendingPass(RendererDX11* renderer) :
      m_depthStencilBuffer(nullptr),
      m_boundSrcBuffer(nullptr),
      m_boundDestBuffer(nullptr),
      m_outputHDRBuffer(nullptr),
      m_params(nullptr),
      RenderingPass(renderer)
   {
   }

   BlendingPass::~BlendingPass()
   {
      SafeDelete(m_depthStencilBuffer);
      SafeDelete(m_outputHDRBuffer);
      SafeDelete(m_params);
   }

   bool BlendingPass::Init(unsigned int width, unsigned int height)
   {
      bool bValidParams = (width > 0) && (height > 0);
      if (bValidParams && RenderingPass::Init(TEXT("Contents/Shaders/Blending.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();

         m_params = new ConstantBufferDX11(renderer);
         if (!m_params->Init<BlendingParams>())
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

   bool BlendingPass::Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* srcBuffer, RenderTargetDX11* destBuffer)
   {
      bool bIsValidParams = srcBuffer != nullptr && destBuffer != nullptr;
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         if (srcBuffer->BindAsShaderResource(deviceContext, 0, EShaderType::PixelShader))
         {
            m_boundSrcBuffer = srcBuffer;
            if (destBuffer->BindAsShaderResource(deviceContext, 1, EShaderType::PixelShader))
            {
               m_boundDestBuffer = destBuffer;
               if (m_outputHDRBuffer->BindAsRenderTarget(deviceContext))
               {
                  if (m_params->Bind(deviceContext, 0, EShaderType::PixelShader))
                  {
                     return true;
                  }
               }
            }
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void BlendingPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         m_boundSrcBuffer->UnbindShaderResource(deviceContext);
         m_boundDestBuffer->UnbindShaderResource(deviceContext);
         m_outputHDRBuffer->UnbindRenderTarget(deviceContext);
         m_params->Unbind(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   void BlendingPass::UpdateParameters(ID3D11DeviceContext& deviceContext, BlendingParams buffer)
   {
      if (m_params != nullptr)
      {
         m_params->Update(deviceContext, buffer);
      }
   }
}
