#include "GBuffer.h"
#include "RenderTargetDX11.h"
#include "DepthStencilBufferDX11.h"

namespace Mile
{
   GBuffer::GBuffer( RendererDX11* renderer ) :
      m_renderer( renderer ),
      m_depthStencilBuffer( nullptr ),
      m_normalBuffer( nullptr ),
      m_positionBuffer( nullptr )
   {
   }

   GBuffer::~GBuffer( )
   {
      SafeDelete( m_normalBuffer );
      SafeDelete( m_positionBuffer );
   }

   bool GBuffer::Init( unsigned int width, unsigned int height )
   {
      bool bIsInitialized = m_normalBuffer != nullptr || m_positionBuffer != nullptr;
      if ( m_renderer == nullptr || bIsInitialized )
      {
         return false;
      }

      m_normalBuffer = new RenderTargetDX11( m_renderer );
      m_positionBuffer = new RenderTargetDX11( m_renderer );

      if ( !m_normalBuffer->Init( width, height, DXGI_FORMAT_R32G32B32A32_FLOAT ) )
      {
         return false;
      }

      if ( !m_positionBuffer->Init( width, height, DXGI_FORMAT_R32G32B32A32_FLOAT ) )
      {
         return false;
      }

      return true;
   }

   bool GBuffer::BindAsRenderTarget( ID3D11DeviceContext& deviceContext )
   {
      bool bIsInitialized = m_normalBuffer != nullptr && m_positionBuffer != nullptr;
      if ( m_renderer == nullptr || !bIsInitialized )
      {
         return false;
      }

      ID3D11DepthStencilView* dsv = nullptr;
      if ( m_depthStencilBuffer != nullptr )
      {
         dsv = m_depthStencilBuffer->GetDSV( );
      }


      std::array<ID3D11RenderTargetView*, 2> targets{
         m_normalBuffer->GetRTV( ),
         m_positionBuffer->GetRTV( )
      };

      const float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
      deviceContext.ClearRenderTargetView( targets[ 0 ], clearColor );
      deviceContext.OMSetRenderTargets( 2, targets.data( ), dsv );

      return true;
   }

   bool GBuffer::BindAsShaderResource( ID3D11DeviceContext& deviceContext, unsigned int startSlot )
   {
      bool bIsInitialized = m_normalBuffer != nullptr && m_positionBuffer != nullptr;
      if ( m_renderer == nullptr || !bIsInitialized )
      {
         return false;
      }

      m_normalBuffer->BindAsShaderResource( deviceContext, startSlot + 0, ShaderType::PixelShader );
      m_positionBuffer->BindAsShaderResource( deviceContext, startSlot + 1, ShaderType::PixelShader );

      return true;
   }

   void GBuffer::UnbindShaderResource( ID3D11DeviceContext& deviceContext )
   {
      bool bIsInitialized = m_normalBuffer != nullptr && m_positionBuffer != nullptr;
      if ( bIsInitialized )
      {
         m_normalBuffer->UnbindShaderResource( deviceContext );
         m_positionBuffer->UnbindShaderResource( deviceContext );

         std::array<ID3D11RenderTargetView*, 2> targets{
            nullptr,
            nullptr,
         };

         deviceContext.OMSetRenderTargets( 2, targets.data( ), nullptr );
      }
   }

   void GBuffer::UnbindRenderTarget( ID3D11DeviceContext& deviceContext )
   {
      bool bIsInitialized = m_normalBuffer != nullptr && m_positionBuffer != nullptr;
      if ( bIsInitialized )
      {
         std::array<ID3D11RenderTargetView*, 2> targets{
            nullptr,
            nullptr,
         };

         deviceContext.OMSetRenderTargets( 2, targets.data( ), nullptr );
      }
   }
}