#pragma once

#include "RenderingPass.h"
#include "Math\Matrix.h"

namespace Mile
{
   class GBuffer;
   class MEAPI GBufferPass : public RenderingPass
   {
      struct alignas( 16 ) TransformConstantBuffer
      {
         Matrix m_worldMatrix;
         Matrix m_worldViewMatrix;
         Matrix m_worldViewProjMatrix;
      };

      struct alignas( 16 ) MaterialConstantBuffer
      {
         float specularExp;
      };

   public:
      GBufferPass( RendererDX11* renderer );
      ~GBufferPass( );

      virtual bool Init( const String& filePath );
      virtual bool Bind( ) override;
      virtual void Unbind( ) override;

      void SetGBuffer( GBuffer* buffer ) { this->m_gBuffer = buffer; }

      void UpdateTransformBuffer( const Matrix& world, const Matrix& view, const Matrix& proj );
      void UpdateMaterialBuffer( float specExp );
      void UpdateNormalTexture( Texture2dDX11* texture );

      CBufferPtr GetTransformBuffer( ) { return m_transformBuffer; }

   private:
      CBufferPtr      m_transformBuffer;
      CBufferPtr      m_materialBuffer;
      Texture2dDX11*           m_normalTexture;
      GBuffer*                 m_gBuffer;

   };
}