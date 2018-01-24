#pragma once

#include "RenderingPass.h"

namespace Mile
{
   class MEAPI TestRenderPass : public RenderingPass
   {
      struct alignas( 16 ) TransformConstantBuffer
      {
         Matrix m_worldMatrix;
         Matrix m_worldViewMatrix;
         Matrix m_worldViewProjMatrix;
      };

   public:
      TestRenderPass( RendererDX11* renderer );
      ~TestRenderPass( );

      virtual bool Init( const String& filePath );
      virtual bool Bind( ID3D11DeviceContext& deviceContext ) override;
      virtual void Unbind( ID3D11DeviceContext& deviceContext ) override;

      void UpdateDiffuseMap( ID3D11DeviceContext& deviceContext, Texture2dDX11* texture );
      void UpdateTransformBuffer( ID3D11DeviceContext& deviceContext, const Matrix& world, const Matrix& view, const Matrix& proj );

   private:
      CBufferPtr      m_transformBuffer;
      Texture2dDX11*  m_diffuseMap;

   };
}