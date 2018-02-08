#include "GBufferPass.h"
#include "GBuffer.h"
#include "ConstantBufferDX11.h"
#include "PixelShaderDX11.h"
#include "Texture2dDX11.h"

namespace Mile
{
   GBufferPass::GBufferPass( RendererDX11* renderer ) :
      m_gBuffer( nullptr ),
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

      m_checkerBoardBuffer = new ConstantBufferDX11( m_renderer );
      if ( !m_checkerBoardBuffer->Init( sizeof( CheckerBoardConstantBuffer ) ) )
      {
         return false;
      }

      m_pixelShader->AddSampler( D3D11_FILTER_ANISOTROPIC,
                                 D3D11_TEXTURE_ADDRESS_BORDER,
                                 D3D11_COMPARISON_ALWAYS );

      return true;
   }

   bool GBufferPass::Bind( ID3D11DeviceContext& deviceContext )
   {
      if ( m_renderer == nullptr )
      {
         return false;
      }
      if ( !RenderingPass::Bind( deviceContext ) )
      {
         return false;
      }

      bool bIsReadyToBind = m_transformBuffer != nullptr &&
         m_materialBuffer != nullptr &&
         m_gBuffer != nullptr;
      if ( !bIsReadyToBind )
      {
         return false;
      }

      if ( !m_gBuffer->BindAsRenderTarget( deviceContext ) )
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

      return true;
   }

   void GBufferPass::Unbind( ID3D11DeviceContext& deviceContext )
   {
      if ( m_renderer == nullptr )
      {
         return;
      }

      if ( m_gBuffer != nullptr )
      {
         m_gBuffer->UnbindRenderTarget( deviceContext );
      }

      if ( m_transformBuffer != nullptr )
      {
         m_transformBuffer->Unbind( deviceContext );
      }

      if ( m_materialBuffer != nullptr )
      {
         m_materialBuffer->Unbind( deviceContext );
      }

      UpdateNormalTexture( deviceContext, nullptr );
      RenderingPass::Unbind( deviceContext );
   }

   void GBufferPass::UpdateTransformBuffer( ID3D11DeviceContext& deviceContext, const Matrix& world, const Matrix& worldView, const Matrix& worldViewProj )
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

   void GBufferPass::UpdateMaterialBuffer( ID3D11DeviceContext& deviceContext, float specExp )
   {
      if ( m_materialBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< MaterialConstantBuffer* >( m_materialBuffer->Map( deviceContext ) );
         buffer->specularExp = specExp;

         m_materialBuffer->UnMap( deviceContext );
      }
   }

   void GBufferPass::UpdateCheckerBoardBuffer( ID3D11DeviceContext& deviceContext, bool checkerBoardEnable )
   {
      if ( m_checkerBoardBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< CheckerBoardConstantBuffer* >( m_checkerBoardBuffer->Map( deviceContext ) );
         buffer->checkerBoardEnable = checkerBoardEnable;

         m_checkerBoardBuffer->UnMap( deviceContext );
      }
   }

   void GBufferPass::UpdateNormalTexture( ID3D11DeviceContext& deviceContext, Texture2dDX11* texture )
   {
      if ( m_normalTexture != nullptr )
      {
         m_normalTexture->Unbind( deviceContext );
      }

      if ( texture != nullptr )
      {
         texture->Unbind( deviceContext );
         m_normalTexture = texture;
         m_normalTexture->Bind( deviceContext, 0, ShaderType::PixelShader );
      }
   }
}