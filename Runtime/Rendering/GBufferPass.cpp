#include "GBufferPass.h"
#include "ConstantBufferDX11.h"

namespace Mile
{
   GBufferPass::GBufferPass( RendererDX11* renderer ) :
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
         SafeDelete( m_transformBuffer );
         return false;
      }

      m_materialBuffer = new ConstantBufferDX11( m_renderer );
      if ( !m_materialBuffer->Init( sizeof( MaterialConstantBuffer ) ) )
      {
         SafeDelete( m_transformBuffer );
         SafeDelete( m_materialBuffer );
         return false;
      }

      return true;
   }

   bool GBufferPass::Bind( )
   {
      bool bIsInitialized = m_transformBuffer != nullptr && m_materialBuffer != nullptr;
      if ( m_renderer == nullptr || !bIsInitialized )
      {
         return false;
      }

      if ( !m_transformBuffer->Bind( 0, ShaderType::VertexShader ) )
      {
         return false;
      }

      if ( !m_materialBuffer->Bind( 1, ShaderType::PixelShader ) )
      {
         return false;
      }

      return true;
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

}