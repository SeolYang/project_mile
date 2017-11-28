#include "GBufferPass.h"
#include "GBuffer.h"
#include "ConstantBufferDX11.h"
#include "PixelShaderDX11.h"
#include "Texture2dDX11.h"

namespace Mile
{
   GBufferPass::GBufferPass( RendererDX11* renderer ) :
      m_normalTexture( nullptr ),
      m_transformBuffer( nullptr ),
      m_materialBuffer( nullptr ),
      RenderingPass( renderer )
   {
   }

   GBufferPass::~GBufferPass( )
   {
      SafeDelete( m_transformBuffer );
      SafeDelete( m_materialBuffer );
   }

   bool GBufferPass::Init( const String& filePath )
   {
      if ( !RenderingPass::Init( filePath ) )
      {
         return false;
      }

      m_transformBuffer = new ConstantBufferDX11( m_renderer );
      if ( !m_transformBuffer->Init( sizeof( TransformConstantBuffer ) ) )
      {
         return false;
      }

      m_materialBuffer = new ConstantBufferDX11( m_renderer );
      if ( !m_materialBuffer->Init( sizeof( MaterialConstantBuffer ) ) )
      {
         return false;
      }

      m_pixelShader->AddSampler( D3D11_FILTER_ANISOTROPIC,
                                 D3D11_TEXTURE_ADDRESS_BORDER, 
                                 D3D11_COMPARISON_ALWAYS
      );

      return true;
   }

   bool GBufferPass::Bind( )
   {
      if ( !RenderingPass::Bind( ) )
      {
         return false;
      }

      bool bIsInitialized = m_transformBuffer != nullptr && m_materialBuffer != nullptr;
      if ( !bIsInitialized  || m_gBuffer == nullptr )
      {
         return false;
      }

      if ( !m_gBuffer->BindAsRenderTarget( ) )
      {
         return false;
      }

      if ( !m_transformBuffer->Bind( 0, ShaderType::VertexShader ) && !m_transformBuffer->Bind( 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_materialBuffer->Bind( 1, ShaderType::VertexShader ) && !m_materialBuffer->Bind( 1, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_normalTexture->Bind( 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      return true;
   }

   void GBufferPass::Unbind( )
   {
      if ( m_renderer == nullptr )
      {
         return;
      }

      if ( m_normalTexture != nullptr )
      {
         m_normalTexture->Unbind( );
      }
   }

   void GBufferPass::UpdateTransformBuffer( const Matrix& world, const Matrix& worldView, const Matrix& worldViewProj )
   {
      if ( m_transformBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< TransformConstantBuffer* >( m_transformBuffer->Map( ) );
         buffer->m_worldMatrix = world;
         buffer->m_worldViewMatrix = worldView;
         buffer->m_worldViewProjMatrix = worldViewProj;

         m_transformBuffer->UnMap( );
      }
   }

   void GBufferPass::UpdateMaterialBuffer( float specExp )
   {
      if ( m_materialBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< MaterialConstantBuffer* >( m_materialBuffer->Map( ) );
         buffer->specularExp = specExp;

         m_materialBuffer->UnMap( );
      }
   }

   void GBufferPass::UpdateNormalTexture( Texture2dDX11* texture )
   {
      if ( texture == nullptr )
      {
         m_normalTexture->Unbind( );
         m_normalTexture = nullptr;
      }
   }
}