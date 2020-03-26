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

   bool GeometryPass::Init(const String& shaderPath)
   {
      if (!RenderingPass::Init(shaderPath))
      {
         return false;
      }

      m_transformBuffer = new ConstantBufferDX11(m_renderer);
      if (!m_transformBuffer->Init(sizeof(TransformConstantBuffer)))
      {
         return false;
      }

      m_materialBuffer = new ConstantBufferDX11(m_renderer);
      if (!m_materialBuffer->Init(sizeof(MaterialConstantBuffer)))
      {
         return false;
      }

      m_pixelShader->AddSampler(
         D3D11_FILTER_ANISOTROPIC,
         D3D11_TEXTURE_ADDRESS_BORDER,
         D3D11_COMPARISON_ALWAYS);

      return true;
   }

   bool GeometryPass::Bind(ID3D11DeviceContext& deviceContext)
   {
      bool bIsNotReadyToBind = !RenderingPass::Bind(deviceContext) ||
         m_transformBuffer == nullptr ||
         m_materialBuffer == nullptr ||
         m_gBuffer == nullptr;
      if (bIsNotReadyToBind)
      {
         return false;
      }

      if (!m_gBuffer->BindAsRenderTarget(deviceContext))
      {
         return false;
      }

      if (!m_transformBuffer->Bind(deviceContext, 0, EShaderType::VertexShader))
      {
         return false;
      }

      if (!m_materialBuffer->Bind(deviceContext, 0, EShaderType::PixelShader))
      {
         return false;
      }

      return true;
   }

   void GeometryPass::Unbind(ID3D11DeviceContext& deviceContext)
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

   void GeometryPass::UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer)
   {
      if (m_transformBuffer != nullptr)
      {
         auto mappedBuffer = reinterpret_cast<TransformConstantBuffer*>(m_transformBuffer->Map(deviceContext));
         (*mappedBuffer) = buffer;
         m_transformBuffer->UnMap(deviceContext);
      }
   }

   void GeometryPass::UpdateMaterialBuffer(ID3D11DeviceContext& deviceContext, MaterialConstantBuffer buffer)
   {
      if (m_materialBuffer != nullptr)
      {
         auto mappedBuffer = reinterpret_cast<MaterialConstantBuffer*>(m_materialBuffer->Map(deviceContext));
         (*mappedBuffer) = buffer;
         m_transformBuffer->UnMap(deviceContext);
      }
   }
}
