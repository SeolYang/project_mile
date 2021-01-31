#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Matrix.h"
#include "Math/Vector4.h"
#include "Math/Vector2.h"

namespace Mile
{
   class GBuffer;
   class MEAPI GeometryPass : public RenderingPass
   {
      DEFINE_CONSTANT_BUFFER(TransformConstantBuffer)
      {
         Matrix WorldMatrix;
         Matrix WorldViewMatrix;
         Matrix WorldViewProjMatrix;
      };

      DEFINE_CONSTANT_BUFFER(MaterialConstantBuffer)
      {
         Vector4 BaseColorFactor;
         Vector4 EmissiveColorFactor;
         float MetallicFactor;
         float RoughnessFactor;
         Vector2 UVOffset;
         float SpecularFactor;
      };

      DEFINE_CONSTANT_BUFFER(PackedMaterialParams)
      {
         Vector4 BaseColorFactor;
         Vector4 EmissiveColorFactor;
         Vector4 MetallicRoughnessUV;
         float SpecularFactor;
      };

   public:
      GeometryPass(RendererDX11* renderer);
      virtual ~GeometryPass();

      bool Init();
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void SetGBuffer(GBuffer* gBuffer) { this->m_gBuffer = gBuffer; }

      void UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, TransformConstantBuffer buffer);
      void UpdateMaterialBuffer(ID3D11DeviceContext& deviceContext, MaterialConstantBuffer buffer);

   private:
      GBuffer* m_gBuffer;
      CBufferPtr m_transformBuffer;
      CBufferPtr m_materialBuffer;

   };
}