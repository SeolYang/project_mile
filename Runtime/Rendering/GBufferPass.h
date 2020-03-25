#pragma once
#include "Rendering/RenderingPass.h"
#include "Math/Matrix.h"

namespace Mile
{
   class GBuffer;
   class MEAPI GBufferPass : public RenderingPass
   {
      struct alignas(16) TransformConstantBuffer
      {
         Matrix m_worldMatrix;
         Matrix m_worldViewMatrix;
         Matrix m_worldViewProjMatrix;
      };

      struct alignas(16) CheckerBoardConstantBuffer
      {
         bool checkerBoardEnable;
      };

   public:
      GBufferPass(RendererDX11* renderer);
      ~GBufferPass();

      virtual bool Init(const String& filePath);
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      void SetGBuffer(GBuffer* buffer) { this->m_gBuffer = buffer; }

      void UpdateTransformBuffer(ID3D11DeviceContext& deviceContext, const Matrix& world, const Matrix& view, const Matrix& proj);
      void UpdateCheckerBoardBuffer(ID3D11DeviceContext& deviceContext, bool checkerBoardEnable);
      void UpdateNormalTexture(ID3D11DeviceContext& deviceContext, Texture2dDX11* texture);

      CBufferPtr GetTransformBuffer() { return m_transformBuffer; }
      CBufferPtr GetCheckerBoardConstantBuffer() { return m_checkerBoardBuffer; }

   private:
      CBufferPtr      m_transformBuffer;
      CBufferPtr      m_materialBuffer;
      CBufferPtr      m_checkerBoardBuffer;
      Texture2dDX11* m_normalTexture;
      GBuffer* m_gBuffer;

   };
}