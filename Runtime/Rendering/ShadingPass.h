#pragma once

#include "RenderingPass.h"

namespace Mile
{
   class MEAPI ShadingPass : public RenderingPass
   {
      struct TransformConstantBuffer
      {
         Matrix m_worldMatrix;
         Matrix m_worldViewMatrix;
         Matrix m_worldViewProjMatrix;
      };

      struct MaterialConstantBuffer
      {
         Vector3 SpecularAlbedo;
      };

   public:
      ShadingPass( RendererDX11* renderer );
      ~ShadingPass( );

      virtual bool Init( const String& filePath ) override;
      virtual bool Bind( ) override;
      virtual void Unbind( ) override;

      void SetLightBuffer( RenderTargetDX11* lightBuffer ) { m_lightBuffer = lightBuffer; }
      void UpdateDiffuseTexture( Texture2dDX11* diffuseTexture );

      // To reuse transform constant buffer from GBufferPass
      void AcquireTransformBuffer( GBufferPass* gBufferPass );

   private:
      CBufferPtr        m_transformBuffer;
      CBufferPtr        m_materialBuffer;

      Texture2dDX11*    m_diffuseTexture;
      RenderTargetDX11* m_lightBuffer;

   };
}