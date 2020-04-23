#include "Rendering/IrradianceConvPass.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/DynamicCubemap.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/Viewport.h"
#include "Rendering/RasterizerState.h"

namespace Mile
{
   IrradianceConvPass::IrradianceConvPass(RendererDX11* renderer) :
      m_viewport(nullptr),
      m_irradianceMap(nullptr),
      m_transformBuffer(nullptr),
      m_boundEnvironmentMap(nullptr),
      RenderingPass(renderer)
   {
   }

   IrradianceConvPass::~IrradianceConvPass()
   {
      SafeDelete(m_viewport);
      SafeDelete(m_transformBuffer);
      SafeDelete(m_irradianceMap);
   }

   bool IrradianceConvPass::Init(unsigned int cubemapSize)
   {
      bool availableParams = cubemapSize > 0;
      if (availableParams &&
         RenderingPass::Init(TEXT("Contents/Shaders/IrradianceConvolution.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();
         m_irradianceMap = new DynamicCubemap(renderer);
         if (!m_irradianceMap->Init(cubemapSize))
         {
            return false;
         }

         m_transformBuffer = new ConstantBufferDX11(renderer);
         if (!m_transformBuffer->Init(sizeof(TransformConstantBuffer)))
         {
            return false;
         }

         m_viewport = new Viewport(renderer);
         m_viewport->SetWidth(static_cast<float>(cubemapSize));
         m_viewport->SetHeight(static_cast<float>(cubemapSize));

         PixelShaderDX11* pixelShader = GetPixelShader();
         pixelShader->AddSampler(
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_TEXTURE_ADDRESS_WRAP,
            D3D11_COMPARISON_ALWAYS);

         RenderObject::ConfirmInit();
         return true;
      }

      return false;
   }

   bool IrradianceConvPass::Bind(ID3D11DeviceContext& deviceContext, Texture2DBaseDX11* environmentMap)
   {
      if (RenderingPass::Bind(deviceContext))
      {
         if (environmentMap != nullptr)
         {
            if (!environmentMap->Bind(deviceContext, 0, EShaderType::PixelShader))
            {
               return false;
            }
         }

         m_boundEnvironmentMap = environmentMap;
         if (!m_viewport->Bind(deviceContext))
         {
            return false;
         }

         if (!m_transformBuffer->Bind(deviceContext, 0, EShaderType::VertexShader))
         {
            return false;
         }

         return true;
      }

      return false;
   }

   void IrradianceConvPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::IsBindable())
      {
         if (m_boundEnvironmentMap != nullptr)
         {
            m_boundEnvironmentMap->Unbind(deviceContext);
            m_boundEnvironmentMap = nullptr;
         }

         m_transformBuffer->Unbind(deviceContext);
         RenderingPass::Unbind(deviceContext);
      }
   }

   void IrradianceConvPass::UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer)
   {
      if (m_transformBuffer != nullptr)
      {
         auto mappedBuffer = reinterpret_cast<TransformConstantBuffer*>(m_transformBuffer->Map(deviceContext));
         if (mappedBuffer != nullptr)
         {
            (*mappedBuffer) = buffer;
         }
         m_transformBuffer->UnMap(deviceContext);
      }
   }
}
