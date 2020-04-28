#include "Rendering/AmbientEmissivePass.h"
#include "Rendering/GBuffer.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/DynamicCubemap.h"
#include "Rendering/BlendState.h"
#include "Rendering/RenderTargetDX11.h"

namespace Mile
{
   AmbientEmissivePass::AmbientEmissivePass(class RendererDX11* renderer) :
      m_gBuffer(nullptr),
      m_irradianceMap(nullptr),
      m_cameraParamsBuffer(nullptr),
      m_ambientParamsBuffer(nullptr),
      RenderingPass(renderer)
   {
   }

   AmbientEmissivePass::~AmbientEmissivePass()
   {
      SafeDelete(m_cameraParamsBuffer);
      SafeDelete(m_ambientParamsBuffer);
   }

   bool AmbientEmissivePass::Init()
   {
      if (!RenderingPass::Init(TEXT("Contents/Shaders/AmbientEmissivePass.hlsl")))
      {
         return false;
      }

      RendererDX11* renderer = GetRenderer();
      m_cameraParamsBuffer = new ConstantBufferDX11(renderer);
      if (!m_cameraParamsBuffer->Init(sizeof(CameraParamsConstantBuffer)))
      {
         return false;
      }

      m_ambientParamsBuffer = new ConstantBufferDX11(renderer);
      if (!m_ambientParamsBuffer->Init(sizeof(AmbientParamsConstantBuffer)))
      {
         return false;
      }

      PixelShaderDX11* pixelShader = GetPixelShader();
      pixelShader->AddSampler(
         D3D11_FILTER_ANISOTROPIC,
         D3D11_TEXTURE_ADDRESS_WRAP,
         D3D11_COMPARISON_ALWAYS);

      RenderObject::ConfirmInit();
      return true;
   }

   bool AmbientEmissivePass::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::Bind(deviceContext))
      {

         if (!m_gBuffer->BindAsShaderResource(deviceContext, 0))
         {
            return false;
         }

         if (!m_irradianceMap->Bind(deviceContext, 5, EShaderType::PixelShader))
         {
            return false;
         }

         if (!m_cameraParamsBuffer->Bind(deviceContext, 0, EShaderType::PixelShader))
         {
            return false;
         }

         if (!m_ambientParamsBuffer->Bind(deviceContext, 1, EShaderType::PixelShader))
         {
            return false;
         }

         return true;
      }

      return false;
   }

   void AmbientEmissivePass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         if (m_gBuffer != nullptr)
         {
            m_gBuffer->UnbindShaderResource(deviceContext);
         }

         if (m_cameraParamsBuffer != nullptr)
         {
            m_cameraParamsBuffer->Unbind(deviceContext);
         }

         if (m_ambientParamsBuffer != nullptr)
         {
            m_ambientParamsBuffer->Unbind(deviceContext);
         }

         if (m_irradianceMap != nullptr)
         {
            m_irradianceMap->Unbind(deviceContext);
         }

         RenderingPass::Unbind(deviceContext);
      }
   }

   void AmbientEmissivePass::SetGBuffer(GBuffer* gBuffer)
   {
      m_gBuffer = gBuffer;
   }

   void AmbientEmissivePass::SetIrradianceMap(DynamicCubemap* irradianceMap)
   {
      m_irradianceMap = irradianceMap;
   }

   void AmbientEmissivePass::UpdateCameraParamsBuffer(ID3D11DeviceContext& deviceContext, CameraParamsConstantBuffer buffer)
   {
      if (m_cameraParamsBuffer != nullptr)
      {
         m_cameraParamsBuffer->Update(deviceContext, buffer);
      }
   }

   void AmbientEmissivePass::UpdateAmbientParamsBuffer(ID3D11DeviceContext& deviceContext, AmbientParamsConstantBuffer buffer)
   {
      if (m_ambientParamsBuffer != nullptr)
      {
         m_ambientParamsBuffer->Update(deviceContext, buffer);
      }
   }
}
