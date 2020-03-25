#pragma once
#include "Rendering/RenderingPass.h"

namespace Mile
{
   class MEAPI ShadingPass : public RenderingPass
   {
      struct alignas(16) TransformConstantBuffer
      {
         Matrix m_worldMatrix;
         Matrix m_worldViewMatrix;
         Matrix m_worldViewProjMatrix;
      };

      // @TODO: PBR Material 에 맞춰서 변경 / constant buffer padding 신경쓰기
      struct alignas(16) MaterialConstantBuffer
      {
         Vector3 SpecularAlbedo;
      };

   public:
      ShadingPass(RendererDX11* renderer);
      ~ShadingPass();

      virtual bool Init(const String& filePath) override;
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void SetLightBuffer(RenderTargetDX11* lightBuffer) { m_lightBuffer = lightBuffer; }
      void SetRenderTarget(RenderTargetDX11* renderTarget) { m_renderTarget = renderTarget; }
      // CheckerBoard Setting does not change after decided at GBuffer-Pass
      void SetCheckerBoardBuffer(CBufferPtr checkerBoardBuffer) { m_checkerBoardBuffer = checkerBoardBuffer; }
      void UpdateDiffuseTexture(ID3D11DeviceContext& deviceContext, Texture2dDX11* diffuseTexture);

      // To reuse transform constant buffer from GBufferPass
      void AcquireTransformBuffer(GBufferPass* gBufferPass);
      void UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, const Matrix& world, const Matrix& worldView, const Matrix& worldViewProj);
      void UpdateMaterialBuffer(ID3D11DeviceContext& deviceContext, const Vector3& specularAlbedo);

   private:
      CBufferPtr        m_transformBuffer;
      CBufferPtr        m_materialBuffer;

      /* External Constant Buffer*/
      ConstantBufferDX11* m_checkerBoardBuffer;

      Texture2dDX11* m_diffuseTexture;
      RenderTargetDX11* m_lightBuffer;

      RenderTargetDX11* m_renderTarget;

   };
}