#include "Rendering/BlendingPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"

namespace Mile
{
   BlendingPass::BlendingPass(RendererDX11* renderer) :
      m_boundSrcBuffer(nullptr),
      m_boundDestBuffer(nullptr),
      m_outputHDRBuffer(nullptr),
      m_params(nullptr),
      RenderingPass(renderer)
   {
   }

   BlendingPass::~BlendingPass()
   {
      SafeDelete(m_outputHDRBuffer);
      SafeDelete(m_params);
   }

   bool BlendingPass::Init(unsigned int width, unsigned int height, DepthStencilBufferDX11* globalDepthStencilBuffer)
   {
      bool bValidParams = (width > 0) && (height > 0);
      if (bValidParams && RenderingPass::Init(TEXT("Contents/Shaders/Blending.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();

         m_params = new ConstantBufferDX11(renderer);
         if (!m_params->Init<PackedBlendingParams>())
         {
            return false;
         }

         m_outputHDRBuffer = new RenderTargetDX11(renderer);
         if (!m_outputHDRBuffer->Init(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, globalDepthStencilBuffer))
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
         bool bSuccess =
            srcBuffer->BindAsShaderResource(deviceContext, 0, EShaderType::PixelShader) &&
            destBuffer->BindAsShaderResource(deviceContext, 1, EShaderType::PixelShader) &&
            m_outputHDRBuffer->BindAsRenderTarget(deviceContext, true, false) &&
            m_params->Bind(deviceContext, 0, EShaderType::PixelShader);
         if (bSuccess)
         {
            m_boundSrcBuffer = srcBuffer;
            m_boundDestBuffer = destBuffer;
            return true;
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
         m_params->Update(deviceContext, PackedBlendingParams{ Vector2(buffer.SrcRatio, buffer.DestRatio) });
      }
   }
}
