#include "RenderTargetDX11.h"
#include "DepthStencilBufferDX11.h"
#include "Texture2dDX11.h"

namespace Mile
{
   RenderTargetDX11::RenderTargetDX11( RendererDX11* renderer ) :
      m_renderer( renderer ),
      m_depthStencilBuffer( nullptr ),
      m_texture( nullptr ),
      m_rtv( nullptr ),
      m_width( 0 ),
      m_height( 0 )
   {
   }

   RenderTargetDX11::~RenderTargetDX11( )
   {
      SafeRelease( m_rtv );
      SafeDelete( m_texture );
   }

   bool RenderTargetDX11::Init( unsigned int width, unsigned int height, DXGI_FORMAT format )
   {
      if ( m_texture != nullptr 
           || m_renderer == nullptr
           || width == 0 || height == 0 )
      {
         return false;
      }

      m_width = width;
      m_height = height;

      D3D11_TEXTURE2D_DESC texDesc;
      ZeroMemory( &texDesc, sizeof( texDesc ) );
      texDesc.Width = width;
      texDesc.Height = height;
      texDesc.MipLevels = 1;
      texDesc.ArraySize = 1;
      texDesc.Format = format;
      texDesc.SampleDesc.Count = 1;
      texDesc.SampleDesc.Quality = 0;
      texDesc.Usage = D3D11_USAGE_DEFAULT;
      texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
      texDesc.CPUAccessFlags = 0;
      texDesc.MiscFlags = 0;

      ID3D11Texture2D* texture = nullptr;
      auto device = m_renderer->GetDevice( );
      auto result = device->CreateTexture2D( &texDesc, 
                                             nullptr,
                                             &texture );
      if ( FAILED( result ) )
      {
         return false;
      }

      D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
      ZeroMemory( &rtvDesc, sizeof( rtvDesc ) );
      rtvDesc.Format = format;
      rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
      rtvDesc.Texture2D.MipSlice = 0;

      result = device->CreateRenderTargetView( texture,
                                               &rtvDesc,
                                               &m_rtv );
      if ( FAILED( result ) )
      {
         return false;
      }

      m_texture = new Texture2dDX11( m_renderer );
      if ( !m_texture->Init( texture ) )
      {
         return false;
      }

      return true;
   }

   bool RenderTargetDX11::BindAsRenderTarget( )
   {
      if ( m_texture == nullptr || m_renderer == nullptr )
      {
         return false;
      }

      ID3D11DepthStencilView* dsv = nullptr;
      if ( m_depthStencilBuffer != nullptr )
      {
         dsv = m_depthStencilBuffer->GetDSV( );
      }

      m_renderer->GetDeviceContext( )->OMSetRenderTargets( 1, &m_rtv, dsv );
      return true;
   }

   bool RenderTargetDX11::BindAsShaderResource( unsigned int startSlot, ShaderType shader )
   {
      if ( m_texture == nullptr )
      {
         return false;
      }

      return m_texture->Bind( startSlot, shader );
   }

   void RenderTargetDX11::UnbindRenderTarget( )
   {
      ID3D11RenderTargetView* nullRTV = nullptr;
      auto deviceContext = m_renderer->GetDeviceContext( );
      deviceContext->OMSetRenderTargets( 1, &nullRTV, nullptr );
   }

   void RenderTargetDX11::UnbindShaderResource( )
   {
      if ( m_texture != nullptr )
      {
         m_texture->Unbind( );
      }
   }
}
