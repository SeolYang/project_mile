#include "Rendering/GeometryPass.h"
#include "Rendering/GBuffer.h"
#include "Rendering/ConstantBufferDX11.h"
#include "Rendering/PixelShaderDX11.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/BlendState.h"

namespace Mile
{
   GeometryPass::GeometryPass(RendererDX11* renderer) :
      m_gBuffer(nullptr),
      m_transformBuffer(nullptr), m_materialBuffer(nullptr),
      RenderingPass(renderer)
   {
   }

   GeometryPass::~GeometryPass()
   {
      SafeDelete(m_transformBuffer);
      SafeDelete(m_materialBuffer);
   }

   bool GeometryPass::Init()
   {
      if (RenderingPass::Init(TEXT("Contents/Shaders/GeometryPass.hlsl")))
      {
         RendererDX11* renderer = GetRenderer();
         m_transformBuffer = new ConstantBufferDX11(renderer);
         if (!m_transformBuffer->Init(sizeof(TransformConstantBuffer)))
         {
            return false;
         }

         m_materialBuffer = new ConstantBufferDX11(renderer);
         if (!m_materialBuffer->Init<PackedMaterialParams>())
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

      return false;
   }

   bool GeometryPass::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderingPass::Bind(deviceContext))
      {
         bool bSuccess =
            m_gBuffer->BindAsRenderTarget(deviceContext) &&
            m_transformBuffer->Bind(deviceContext, 0, EShaderType::VertexShader) &&
            m_materialBuffer->Bind(deviceContext, 0, EShaderType::PixelShader);
         if (bSuccess)
         {
            return true;
         }
      }

      Unbind(deviceContext);
      return false;
   }

   void GeometryPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
      {
         if (m_gBuffer != nullptr)
         {
            m_gBuffer->UnbindRenderTarget(deviceContext);
         }

         if (m_transformBuffer != nullptr)
         {
            m_transformBuffer->Unbind(deviceContext);
         }

         if (m_materialBuffer != nullptr)
         {
            m_materialBuffer->Unbind(deviceContext);
         }

         RenderingPass::Unbind(deviceContext);
      }
   }

   void GeometryPass::UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer)
   {
      if (m_transformBuffer != nullptr)
      {
         m_transformBuffer->Update(deviceContext, buffer);
      }
   }

   void GeometryPass::UpdateMaterialBuffer(ID3D11DeviceContext& deviceContext, MaterialConstantBuffer buffer)
   {
      if (m_materialBuffer != nullptr)
      {
         m_materialBuffer->Update(deviceContext, PackedMaterialParams{
            buffer.BaseColorFactor, 
            buffer.EmissiveColorFactor, 
            Vector4(buffer.MetallicFactor, buffer.RoughnessFactor, buffer.UVOffset.x, buffer.UVOffset.y),
            buffer.SpecularFactor });
      }
   }
}
