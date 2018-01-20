#include "TestRenderPass.h"
#include "ConstantBufferDX11.h"
#include "PixelShaderDX11.h"
#include "Texture2dDX11.h"

namespace Mile
{
   TestRenderPass::TestRenderPass( RendererDX11* renderer ) :
      m_transformBuffer( nullptr ),
      m_diffuseMap( nullptr ),
      RenderingPass( renderer )
   {
   }

   TestRenderPass::~TestRenderPass( )
   {
      Unbind( );
      SafeDelete( m_transformBuffer );
   }

   bool TestRenderPass::Init( const String& filePath )
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


      m_pixelShader->AddSampler( D3D11_FILTER_ANISOTROPIC,
                                 D3D11_TEXTURE_ADDRESS_BORDER,
                                 D3D11_COMPARISON_ALWAYS );

      return true;
   }

   bool TestRenderPass::Bind( )
   {
      if ( !RenderingPass::Bind( ) )
      {
         return false;
      }

      if ( !m_transformBuffer->Bind( 0, ShaderType::VertexShader ) && !m_transformBuffer->Bind( 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      return true;
   }

   void TestRenderPass::Unbind( )
   {
      RenderingPass::Unbind( );
   }

   void TestRenderPass::UpdateDiffuseMap( Texture2dDX11* texture )
   {
      if ( m_diffuseMap != nullptr )
      {
         m_diffuseMap->Unbind( );
         m_diffuseMap = nullptr;
      }

      if ( texture != nullptr )
      {
         texture->Unbind( );
         m_diffuseMap = texture;
         m_diffuseMap->Bind( 0, ShaderType::PixelShader );
      }
   }

   void TestRenderPass::UpdateTransformBuffer( const Matrix& world, const Matrix& worldView, const Matrix& worldViewProj )
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
}