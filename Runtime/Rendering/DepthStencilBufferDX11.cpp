#include "DepthStencilBufferDX11.h"
#include "RendererDX11.h"

namespace Mile
{
   DepthStencilBufferDX11::DepthStencilBufferDX11( RendererDX11* renderer ) :
      m_bStencilEnabled( true ),
      ResourceDX11( renderer )
   {

   }

   DepthStencilBufferDX11::~DepthStencilBufferDX11( )
   {
      SafeRelease( m_depthStencilView );
      SafeRelease( m_depthStencilBuffer );
   }

   bool DepthStencilBufferDX11::Init( unsigned int width, unsigned int height, bool bStencilEnable )
   {
      if ( m_bIsInitialized || m_renderer == nullptr )
      {
         return false;
      }

      m_bStencilEnabled = bStencilEnable;

      D3D11_TEXTURE2D_DESC bufferDesc;
      ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
      bufferDesc.Width = width;
      bufferDesc.Height = height;
      bufferDesc.MipLevels = 1;
      bufferDesc.ArraySize = 1;
      bufferDesc.Format = bStencilEnable ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
      bufferDesc.SampleDesc.Count = 1;
      bufferDesc.SampleDesc.Quality = 0;
      bufferDesc.Usage = D3D11_USAGE_DEFAULT;
      bufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
      bufferDesc.CPUAccessFlags = 0;
      bufferDesc.MiscFlags = 0;

      auto device = m_renderer->GetDevice( );
      auto result = device->CreateTexture2D( &bufferDesc,
                                             nullptr,
                                             &m_depthStencilBuffer );
      if ( FAILED( result ) )
      {
         /* Failed to create Depth-Stencil Buffer. **/
         return false;
      }

      D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
      ZeroMemory( &dsvDesc, sizeof( dsvDesc ) );
      dsvDesc.Format = bStencilEnable ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
      dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
      dsvDesc.Texture2D.MipSlice = 0;

      result = device->CreateDepthStencilView( m_depthStencilBuffer, &dsvDesc, &m_depthStencilView );
      if ( FAILED( result ) )
      {
         return false;
      }

      m_bIsInitialized = true;
      return true;
   }
}