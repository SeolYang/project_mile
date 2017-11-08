#include "RenderTargetDX11.h"

namespace Mile
{
   bool RenderTargetDX11::Init( unsigned int width, unsigned int height )
   {
      if ( m_bIsInitialized 
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
      texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
      texDesc.SampleDesc.Count = 1;
      texDesc.SampleDesc.Quality = 0;
      texDesc.Usage = D3D11_USAGE_DEFAULT;
      texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
      texDesc.CPUAccessFlags = 0;
      texDesc.MiscFlags = 0;

      auto device = m_renderer->GetDevice( );
      auto result = device->CreateTexture2D( &texDesc, 
                                                               nullptr,
                                                               &m_renderTarget );
      if ( FAILED( result ) )
      {
         return false;
      }

      D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
      ZeroMemory( &rtvDesc, sizeof( rtvDesc ) );
      rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
      rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
      rtvDesc.Texture2D.MipSlice = 0;

      result = device->CreateRenderTargetView( m_renderTarget,
                                               &rtvDesc,
                                               &m_renderTargetView );
      if ( FAILED( result ) )
      {
         return false;
      }

      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
      ZeroMemory( &srvDesc, sizeof( srvDesc ) );
      srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
      srvDesc.Texture2D.MostDetailedMip = 0;
      srvDesc.Texture2D.MipLevels = 1;

      m_bIsInitialized = true;
      return true;
   }

   bool RenderTargetDX11::SetAsRenderTarget( )
   {
      if ( !m_bIsInitialized || m_renderer == nullptr )
      {
         return false;
      }

      m_renderer->GetDeviceContext( )->OMSetRenderTargets( 1, &m_renderTargetView, nullptr );
      return true;
   }
}

