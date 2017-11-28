#include "ShadingPass.h"
#include "Texture2dDX11.h"
#include "ConstantBufferDX11.h"
#include "RenderTargetDX11.h"

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
         m_diffuseTexture != nullptr &&
         m_lightBuffer != nullptr;
      if ( !bIsReadyToBind )
      {
         return false;
      }

      if ( !m_transformBuffer->Bind( 0, ShaderType::VertexShader ) 
           || !m_transformBuffer->Bind( 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_materialBuffer->Bind( 1, ShaderType::VertexShader ) 
           || !m_materialBuffer->Bind( 1, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_diffuseTexture->Bind( 0, ShaderType::PixelShader ) )
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
      RenderingPass::Unbind( );

      if ( m_renderer == nullptr )
      {
         return;
      }

      if ( m_diffuseTexture != nullptr )
      {
         m_diffuseTexture->Unbind( );
      }

      if ( m_lightBuffer != nullptr )
      {
         m_lightBuffer->Unbind( );
      }
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
      }
   }

}