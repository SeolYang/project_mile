#include "Rendering/ToneMappingPass.h"
#include "Rendering/RenderTargetDX11.h"
#include "Rendering/PixelShaderDX11.h"

namespace Mile
{
   ToneMappingPass::ToneMappingPass(RendererDX11* renderer) :
      m_boundHdrBuffer(nullptr),
      RenderingPass(renderer)
   {
   }

   ToneMappingPass::~ToneMappingPass()
   {
   }

   bool ToneMappingPass::Init()
   {
      if (RenderingPass::Init(TEXT("Contents/Shaders/ToneMapping.hlsl")))
      {
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

   bool ToneMappingPass::Bind(ID3D11DeviceContext& deviceContext, RenderTargetDX11* hdrBuffer)
   {
      bool bIsValidParams = hdrBuffer != nullptr;
      if (bIsValidParams && RenderingPass::Bind(deviceContext))
      {
         if (!hdrBuffer->BindAsShaderResource(deviceContext, 0, EShaderType::PixelShader))
         {
            return false;
         }
         m_boundHdrBuffer = hdrBuffer;

         return true;
      }

      return false;
   }

   void ToneMappingPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         m_boundHdrBuffer->UnbindShaderResource(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }
}

