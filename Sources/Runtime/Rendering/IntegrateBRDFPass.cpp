#include "Rendering/IntegrateBRDFPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/DepthStencilBufferDX11.h"
#include "Rendering/Viewport.h"

namespace Mile
{
   IntegrateBRDFPass::IntegrateBRDFPass(RendererDX11* renderer) :
      m_brdfLUT(nullptr),
      m_depthStencil(nullptr),
      RenderingPass(renderer)
   {
   }

   IntegrateBRDFPass::~IntegrateBRDFPass()
   {
      SafeDelete(m_viewport);
      SafeDelete(m_depthStencil);
      SafeDelete(m_brdfLUT);
   }

   bool IntegrateBRDFPass::Init(unsigned int lutSize)
   {
      if (RenderingPass::Init(TEXT("Contents/Shaders/IntegrateBRDF.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();

         m_viewport = new Viewport(renderer);
         m_viewport->SetWidth(static_cast<float>(lutSize));
         m_viewport->SetHeight(static_cast<float>(lutSize));

         m_depthStencil = new DepthStencilBufferDX11(renderer);
         if (!m_depthStencil->Init(lutSize, lutSize, false))
         {
            return false;
         }

         m_brdfLUT = new RenderTargetDX11(renderer);
         if (!m_brdfLUT->Init(lutSize, lutSize, EColorFormat::R16G16_FLOAT, m_depthStencil))
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

   bool IntegrateBRDFPass::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::Bind(deviceContext))
      {
         bool bSuccess =
            m_viewport->Bind(deviceContext) &&
            m_brdfLUT->BindAsRenderTarget(deviceContext);
         if (bSuccess)
         {
            return true;
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void IntegrateBRDFPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         m_brdfLUT->UnbindRenderTarget(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }
}
