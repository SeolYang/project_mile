#include "Rendering/SSAOBlurPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/GBuffer.h"

namespace Mile
{
   SSAOBlurPass::SSAOBlurPass(RendererDX11* renderer) :
      m_boundSSAOInput(nullptr),
      m_outputBuffer(nullptr),
      RenderingPass(renderer)
   {
   }

   SSAOBlurPass::~SSAOBlurPass()
   {
      SafeDelete(m_outputBuffer);
   }

   bool SSAOBlurPass::Init(unsigned int width, unsigned int height, DepthStencilBufferDX11* globalDepthStencilBuffer)
   {
      bool bValidParams = (width > 0) && (height > 0);
      if (bValidParams && RenderingPass::Init(TEXT("Contents/Shaders/SSAOBlur.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();

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

         RenderObject::ConfirmInit();
         return true;
      }

      return false;
   }

   bool SSAOBlurPass::Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* ssaoInput)
   {
      bool bIsValidParams = ssaoInput != nullptr;
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         bool bSuccess =
            ssaoInput->BindAsShaderResource(deviceContext, 0, EShaderType::PixelShader) &&
            m_outputBuffer->BindAsRenderTarget(deviceContext, true, false);
         if (bSuccess)
         {
            m_boundSSAOInput = ssaoInput;
            return true;
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void SSAOBlurPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         if (m_boundSSAOInput != nullptr)
         {
            m_boundSSAOInput->UnbindShaderResource(deviceContext);
            m_boundSSAOInput = nullptr;
         }

         m_outputBuffer->UnbindRenderTarget(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }
}