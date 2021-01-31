#include "Rendering/ConvertGBufferToViewPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/GBuffer.h"

namespace Mile
{
   ConvertGBufferToViewPass::ConvertGBufferToViewPass(RendererDX11* renderer) :
      m_boundGBuffer(nullptr),
      m_convertedGBuffer(nullptr),
      m_params(nullptr),
      RenderingPass(renderer)
   {
   }

   ConvertGBufferToViewPass::~ConvertGBufferToViewPass()
   {
      SafeDelete(m_convertedGBuffer);
      SafeDelete(m_params);
   }

   bool ConvertGBufferToViewPass::Init(unsigned int width, unsigned int height, DepthStencilBufferDX11* globalDepthStencilBuffer)
   {
      bool bValidParams = (width > 0) && (height > 0);
      if (bValidParams && RenderingPass::Init(TEXT("Contents/Shaders/ViewSpaceGBuffer.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();

         m_params = new ConstantBufferDX11(renderer);
         if (!m_params->Init<ConvertParams>())
         {
            return false;
         }

         m_convertedGBuffer = new GBuffer(renderer);
         if (!m_convertedGBuffer->Init(width, height))
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

   bool ConvertGBufferToViewPass::Bind(ID3D11DeviceContext& deviceContext, GBuffer* gBuffer)
   {
      bool bIsValidParams = gBuffer != nullptr;
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         bool bSuccess =
            gBuffer->BindAsShaderResource(deviceContext, 0, true) &&
            m_convertedGBuffer->BindAsRenderTarget(deviceContext, true) &&
            m_params->Bind(deviceContext, 0, EShaderType::PixelShader);
         if (bSuccess)
         {
            m_boundGBuffer = gBuffer;
            return true;
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void ConvertGBufferToViewPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         if (m_boundGBuffer != nullptr)
         {
            m_boundGBuffer->UnbindShaderResource(deviceContext);
            m_boundGBuffer = nullptr;
         }

         m_convertedGBuffer->UnbindRenderTarget(deviceContext);
         m_params->Unbind(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   void ConvertGBufferToViewPass::UpdateParameters(ID3D11DeviceContext& deviceContext, ConvertParams buffer)
   {
      if (m_params != nullptr)
      {
         m_params->Update(deviceContext, buffer);
      }
   }
}
