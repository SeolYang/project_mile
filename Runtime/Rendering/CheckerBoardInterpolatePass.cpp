#include "CheckerBoardInterpolatePass.h"
#include "ConstantBufferDX11.h"
#include "RenderTargetDX11.h"

namespace Mile
{
   CheckerBoardInterpolatePass::CheckerBoardInterpolatePass( RendererDX11* renderer ) :
      m_renderTargetInfoBuffer( nullptr ),
      m_checkerBoard( nullptr ),
      m_renderTarget( nullptr ),
      RenderingPass( renderer )
   {
   }

   CheckerBoardInterpolatePass::~CheckerBoardInterpolatePass( )
   {
      SafeDelete( m_renderTargetInfoBuffer );
   }

   bool CheckerBoardInterpolatePass::Init( const String& filePath )
   {
      if ( !RenderingPass::Init( filePath ) )
      {
         return false;
      }

      m_renderTargetInfoBuffer = new ConstantBufferDX11( m_renderer );
      if ( !m_renderTargetInfoBuffer->Init( sizeof( RenderTargetInfoBuffer ) ) )
      {
         return false;
      }

      return true;
   }

   bool CheckerBoardInterpolatePass::Bind( ID3D11DeviceContext& deviceContext )
   {
      if ( !RenderingPass::Bind( deviceContext ) )
      {
         return false;
      }

      bool bReadyToBind =
         m_renderTargetInfoBuffer != nullptr &&
         m_checkerBoard != nullptr &&
         m_renderTarget != nullptr;

      if ( !bReadyToBind )
      {
         return false;
      }

      if ( !m_renderTargetInfoBuffer->Bind( deviceContext, 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_checkerBoard->BindAsShaderResource( deviceContext, 0, ShaderType::PixelShader ) )
      {
         return false;
      }

      if ( !m_renderTarget->BindAsRenderTarget( deviceContext ) )
      {
         return false;
      }

      return true;
   }

   void CheckerBoardInterpolatePass::Unbind( ID3D11DeviceContext& deviceContext )
   {
      if ( m_renderer == nullptr )
      {
         return;
      }

      m_renderTargetInfoBuffer->Unbind( deviceContext );

      if ( m_checkerBoard != nullptr )
      {
         m_checkerBoard->UnbindShaderResource( deviceContext );
      }

      if ( m_renderTarget != nullptr )
      {
         m_renderTarget->UnbindRenderTarget( deviceContext );
      }
   }

   void CheckerBoardInterpolatePass::UpdateRenderTargetInfoBuffer( ID3D11DeviceContext& deviceContext, int width, int height )
   {
      if ( m_renderTargetInfoBuffer != nullptr )
      {
         auto buffer = reinterpret_cast< RenderTargetInfoBuffer* >( m_renderTargetInfoBuffer->Map( deviceContext ) );
         buffer->Width = width;
         buffer->Height = height;
         m_renderTargetInfoBuffer->UnMap( deviceContext );
      }
   }
}