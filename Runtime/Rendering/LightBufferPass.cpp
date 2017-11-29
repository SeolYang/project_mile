#include "LightBufferPass.h"
#include "ConstantBufferDX11.h"
#include "Texture2dDX11.h"
#include "GBuffer.h"
#include "RenderTargetDX11.h"

namespace Mile
{
   LightBufferPass::LightBufferPass( RendererDX11* renderer ) :
      m_lightBuffer( nullptr ),
      m_cameraBuffer( nullptr ),
      m_gBuffer( nullptr ),
      RenderingPass( renderer )
   {
   }

   LightBufferPass::~LightBufferPass( )
   {
      SafeDelete( m_lightParamBuffer );
      SafeDelete( m_cameraBuffer );
   }

   bool LightBufferPass::Init( const String& filePath )
   {
      bool bIsInitialized = m_lightParamBuffer != nullptr || m_cameraBuffer != nullptr;
      if ( m_renderer == nullptr || bIsInitialized )
      {
         return false;
      }

      m_lightParamBuffer = new ConstantBufferDX11( m_renderer );
      if ( !m_lightParamBuffer->Init( sizeof( LightParamConstantBuffer ) ) )
      {
         return false;
      }

      m_cameraBuffer = new ConstantBufferDX11( m_renderer );
      if ( !m_cameraBuffer->Init( sizeof( CameraConstantBuffer ) ) )
      {
         return false;
      }

      return true;
   }

   bool LightBufferPass::Bind( )
   {
      if ( !RenderingPass::Bind( ) )
      {
         return false;
      }

      bool bIsReadyToBind = m_lightBuffer != nullptr &&
         m_cameraBuffer != nullptr &&
         m_gBuffer != nullptr;
      if ( !bIsReadyToBind )
      {
         return false;
      }

      if ( !m_lightBuffer->BindAsRenderTarget( ) )
      {
         return false;
      }

      if ( !m_gBuffer->BindAsShaderResource( 0 ) )
      {
         return false;
      }

      if ( !m_lightParamBuffer->Bind( 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_cameraBuffer->Bind( 1, ShaderType::PixelShader ) )
      {
         return false;
      }

      return true;
   }

   void LightBufferPass::Unbind( )
   {
      RenderingPass::Unbind( );

      if ( m_renderer == nullptr )
      {
         return;
      }

      m_gBuffer->Unbind( );
   }

   void LightBufferPass::SetGBuffer( GBuffer* gBuffer )
   {
      m_gBuffer = gBuffer;
   }

   void LightBufferPass::SetLightBuffer( RenderTargetDX11* lightBuffer )
   {
      m_lightBuffer = lightBuffer;
   }

   void LightBufferPass::UpdateLightParamBuffer( const Vector3& lightPos,
                                            const Vector3& lightColor,
                                            const Vector3& lightDirection,
                                            const Vector2& spotlightAngles,
                                            const Vector3& lightRange,
                                            unsigned int lightType )
   {
      if ( m_lightBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< LightParamConstantBuffer* >( m_lightParamBuffer->Map( ) );
         buffer->LightPos = lightPos;
         buffer->LightColor = lightColor;
         buffer->LightDirection = lightDirection;
         buffer->SpotlightAngles = spotlightAngles;
         buffer->LightRange = lightRange;
         buffer->LightType = lightType;

         m_lightParamBuffer->UnMap( );
      }
   }

   void LightBufferPass::UpdateCameraBuffer( const Vector3& camPos )
   {
      if ( m_cameraBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< CameraConstantBuffer* >( m_cameraBuffer->Map( ) );
         buffer->CameraPos = camPos;

         m_cameraBuffer->UnMap( );
      }
   }
}