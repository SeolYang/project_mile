#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Matrix.h"
#include "Math/Vector4.h"

namespace Mile
{
   class GBuffer;
   class MEAPI GeometryPass : public RenderingPass
   {
      struct alignas(16) TransformConstantBuffer
      {
         Matrix WorldMatrix;
         Matrix WorldViewMatrix;
         Matrix WorldViewProjMatrix;
      };

      struct alignas(16) MaterialConstantBuffer
      {
         Vector4 BaseColorFactor;
         Vector4 EmissiveColorFactor;
         float metallicFactor;
         float roughnessFactor;
      };

   public:
      GeometryPass(RendererDX11* renderer);
      ~GeometryPass();

      virtual bool Init(const String& shaderPath) override;
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void SetGBuffer(GBuffer* gBuffer) { this->m_gBuffer = gBuffer; }

      void UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer);
      void UpdateMaterialBuffer(ID3D11DeviceContext& deviceContext, MaterialConstantBuffer buffer);

      void SetBaseColorMap(ID3D11DeviceContext& deviceContext, Texture2dDX11* texture);
      void SetEmissiveMap(ID3D11DeviceContext& deviceContext, Texture2dDX11* texture);
      void SetMetallicRoughnessMap(ID3D11DeviceContext& deviceContext, Texture2dDX11* texture);
      void SetAOMap(ID3D11DeviceContext& deviceContext, Texture2dDX11* texture);
      void SetNormalMap(ID3D11DeviceContext& deviceContext, Texture2dDX11* texture);

   private:
      GBuffer* m_gBuffer;
      CBufferPtr m_transformBuffer;
      CBufferPtr m_materialBuffer;

      Texture2dDX11* m_baseColorTex;
      Texture2dDX11* m_emissiveTex;
      Texture2dDX11* m_metallicRoughnessTex;
      Texture2dDX11* m_aoTex;
      Texture2dDX11* m_normalTex;

   };
}