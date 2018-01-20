#include "ShadingPass.h"
#include "GBufferPass.h"
#include "ConstantBufferDX11.h"
#include "Texture2dDX11.h"
#include "RenderTargetDX11.h"
#include "PixelShaderDX11.h"

namespace Mile
{
   ShadingPass::ShadingPass( RendererDX11* renderer ) :
      m_transformBuffer( nullptr ),
      m_materialBuffer( nullptr ),
      m_diffuseTexture( nullptr ),
      RenderingPass( renderer )
   {
   }

   ShadingPass::~ShadingPass( )
   {
      // Do not delete transform constant buffer in shading pass destructor.
      // Because it is acquire from G-Buffer Pass(recycle constant buffer object)
      SafeDelete( m_materialBuffer );
   }

   bool ShadingPass::Init( const String& filePath )
   {
      if ( !RenderingPass::Init( filePath ) )
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
                                 D3D11_COMPARISON_ALWAYS );

      return true;
   }

   bool ShadingPass::Bind( )
   {
      if ( !RenderingPass::Bind( ) )
      {
         return false;
      }

      bool bIsReadyToBind = m_transformBuffer != nullptr &&
         m_materialBuffer != nullptr &&
         m_lightBuffer != nullptr;
      if ( !bIsReadyToBind )
      {
         return false;
      }

      if ( !m_transformBuffer->Bind( 0, ShaderType::VertexShader ) )
      {
         return false;
      }

      if ( !m_materialBuffer->Bind( 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_lightBuffer->BindAsShaderResource( 1, ShaderType::PixelShader ) )
      {
         return false;
      }

      return true;
   }

   void ShadingPass::Unbind( )
   {
      if ( m_renderer == nullptr )
      {
         return;
      }

      m_transformBuffer->Unbind( );
      m_materialBuffer->Unbind( );

      if ( m_diffuseTexture != nullptr )
      {
         m_diffuseTexture->Unbind( );
      }

      if ( m_lightBuffer != nullptr )
      {
         m_lightBuffer->UnbindShaderResource( );
      }

      RenderingPass::Unbind( );
   }

   void ShadingPass::UpdateDiffuseTexture( Texture2dDX11* diffuseTexture )
   {
      if ( m_diffuseTexture != nullptr )
      {
         m_diffuseTexture->Unbind( );
      }

      if ( diffuseTexture != nullptr )
      {
         diffuseTexture->Unbind( );
         m_diffuseTexture = diffuseTexture;
         m_diffuseTexture->Bind( 0, ShaderType::PixelShader );
      }
   }

   void ShadingPass::AcquireTransformBuffer( GBufferPass* gBufferPass )
   {
      if ( gBufferPass != nullptr )
      {
         m_transformBuffer = gBufferPass->GetTransformBuffer( );
      }
   }

   void ShadingPass::UpdateTransformBuffer( const Matrix& world, const Matrix& worldView, const Matrix& worldViewProj )
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

   void ShadingPass::UpdateMaterialBuffer( const Vector3& specAlbedo )
   {
      if ( m_materialBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< MaterialConstantBuffer* >( m_materialBuffer->Map( ) );
         buffer->SpecularAlbedo = specAlbedo;
         m_materialBuffer->UnMap( );
      }
   }
}