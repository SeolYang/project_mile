#include "LightBufferPass.h"
#include "ConstantBufferDX11.h"
#include "Texture2dDX11.h"
#include "GBuffer.h"
#include "RenderTargetDX11.h"

namespace Mile
{
   LightBufferPass::LightBufferPass( RendererDX11* renderer ) :
      m_lightBuffer( nullptr ),
      m_lightParamBuffer( nullptr ),
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

      if ( !RenderingPass::Init( filePath ) )
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

   bool LightBufferPass::Bind( ID3D11DeviceContext& deviceContext )
   {
      if ( !RenderingPass::Bind( deviceContext ) )
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

      if ( !m_lightBuffer->BindAsRenderTarget( deviceContext ) )
      {
         return false;
      }

      if ( !m_gBuffer->BindAsShaderResource( deviceContext, 0 ) )
      {
         return false;
      }

      if ( !m_lightParamBuffer->Bind( deviceContext, 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_cameraBuffer->Bind( deviceContext, 1, ShaderType::PixelShader ) )
      {
         return false;
      }

      return true;
   }

   void LightBufferPass::Unbind( ID3D11DeviceContext& deviceContext )
   {
      if ( m_renderer == nullptr )
      {
         return;
      }

      m_lightParamBuffer->Unbind( deviceContext );
      m_cameraBuffer->Unbind( deviceContext );
      m_lightBuffer->UnbindRenderTarget( deviceContext );
      m_gBuffer->UnbindShaderResource( deviceContext );
      RenderingPass::Unbind( deviceContext );
   }

   void LightBufferPass::SetGBuffer( GBuffer* gBuffer )
   {
      m_gBuffer = gBuffer;
   }

   void LightBufferPass::SetLightBuffer( RenderTargetDX11* lightBuffer )
   {
      m_lightBuffer = lightBuffer;
   }

   void LightBufferPass::UpdateLightParamBuffer( ID3D11DeviceContext& deviceContext, const Vector3& lightPos,
                                            const Vector3& lightColor,
                                            const Vector3& lightDirection,
                                            const Vector2& spotlightAngles,
                                            const Vector3& lightRange,
                                            unsigned int lightType )
   {
      if ( m_lightBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< LightParamConstantBuffer* >( m_lightParamBuffer->Map( deviceContext ) );
         buffer->LightPos = lightPos;
         buffer->LightColor = lightColor;
         buffer->LightDirection = lightDirection;
         buffer->SpotlightAngles = spotlightAngles;
         buffer->LightRange = lightRange;
         buffer->LightType = lightType;

         m_lightParamBuffer->UnMap( deviceContext );
      }
   }

   void LightBufferPass::UpdateCameraBuffer( ID3D11DeviceContext& deviceContext, const Vector3& camPos )
   {
      if ( m_cameraBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< CameraConstantBuffer* >( m_cameraBuffer->Map( deviceContext ) );
         buffer->CameraPos = camPos;

         m_cameraBuffer->UnMap( deviceContext );
      }
   }
}