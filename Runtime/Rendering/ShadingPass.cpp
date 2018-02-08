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

   bool ShadingPass::Bind( ID3D11DeviceContext& deviceContext )
   {
      if ( !RenderingPass::Bind( deviceContext ) )
      {
         return false;
      }

      bool bIsReadyToBind = m_transformBuffer != nullptr &&
         m_materialBuffer != nullptr &&
         m_lightBuffer != nullptr &&
         m_renderTarget != nullptr;
      if ( !bIsReadyToBind )
      {
         return false;
      }

      if ( !m_transformBuffer->Bind( deviceContext, 0, ShaderType::VertexShader ) )
      {
         return false;
      }

      if ( !m_materialBuffer->Bind( deviceContext, 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_checkerBoardBuffer->Bind( deviceContext, 1, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_lightBuffer->BindAsShaderResource( deviceContext, 1, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_renderTarget->BindAsRenderTarget( deviceContext ) )
      {
         return false;
      }

      return true;
   }

   void ShadingPass::Unbind( ID3D11DeviceContext& deviceContext )
   {
      if ( m_renderer == nullptr )
      {
         return;
      }

      m_transformBuffer->Unbind( deviceContext );
      m_materialBuffer->Unbind( deviceContext );

      if ( m_checkerBoardBuffer != nullptr )
      {
         m_checkerBoardBuffer->Unbind( deviceContext );
      }

      if ( m_diffuseTexture != nullptr )
      {
         m_diffuseTexture->Unbind( deviceContext );
      }

      if ( m_lightBuffer != nullptr )
      {
         m_lightBuffer->UnbindShaderResource( deviceContext );
      }

      if ( m_renderTarget != nullptr )
      {
         m_renderTarget->UnbindRenderTarget( deviceContext );
      }

      RenderingPass::Unbind( deviceContext );
   }

   void ShadingPass::UpdateDiffuseTexture( ID3D11DeviceContext& deviceContext, Texture2dDX11* diffuseTexture )
   {
      if ( m_diffuseTexture != nullptr )
      {
         m_diffuseTexture->Unbind( deviceContext );
      }

      if ( diffuseTexture != nullptr )
      {
         diffuseTexture->Unbind( deviceContext );
         m_diffuseTexture = diffuseTexture;
         m_diffuseTexture->Bind( deviceContext, 0, ShaderType::PixelShader );
      }
   }

   void ShadingPass::AcquireTransformBuffer( GBufferPass* gBufferPass )
   {
      if ( gBufferPass != nullptr )
      {
         m_transformBuffer = gBufferPass->GetTransformBuffer( );
      }
   }

   void ShadingPass::UpdateTransformBuffer( ID3D11DeviceContext& deviceContext, const Matrix& world, const Matrix& worldView, const Matrix& worldViewProj )
   {
      if ( m_transformBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< TransformConstantBuffer* >( m_transformBuffer->Map( deviceContext ) );
         buffer->m_worldMatrix = world;
         buffer->m_worldViewMatrix = worldView;
         buffer->m_worldViewProjMatrix = worldViewProj;
         m_transformBuffer->UnMap( deviceContext );
      }
   }

   void ShadingPass::UpdateMaterialBuffer( ID3D11DeviceContext& deviceContext, const Vector3& specAlbedo )
   {
      if ( m_materialBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< MaterialConstantBuffer* >( m_materialBuffer->Map( deviceContext ) );
         buffer->SpecularAlbedo = specAlbedo;
         m_materialBuffer->UnMap( deviceContext );
      }
   }
}