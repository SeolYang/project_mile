#include "Texture2dDX11.h"
#include "RendererDX11.h"

namespace Mile
{
   bool Texture2dDX11::Init( unsigned int width, unsigned int height, int channels, unsigned char* data, DXGI_FORMAT format )
   {
      if ( m_bIsInitialized || m_renderer == nullptr
           || width == 0 || height == 0 )
      {
         return false;
      }

      auto device = m_renderer->GetDevice( );

      D3D11_TEXTURE2D_DESC desc;
      ZeroMemory( &desc, sizeof( desc ) );
      desc.Width = width;
      desc.Height = height;
      desc.MipLevels = m_mipLevels;
      desc.ArraySize = 1;
      desc.Format = format;
      desc.SampleDesc.Count = 1;
      desc.SampleDesc.Quality = 0;
      desc.Usage = D3D11_USAGE_IMMUTABLE;
      desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      desc.MiscFlags = 0;
      desc.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA resource;
      resource.pSysMem = data;
      resource.SysMemPitch = ( width * channels ) * sizeof( unsigned char );
      resource.SysMemSlicePitch = ( width * height * channels ) * sizeof( unsigned char );

      auto result = device->CreateTexture2D( &desc, &resource, &m_texture );
      if ( FAILED( result ) )
      {
         return false;
      }

      if ( !InitSRV( desc ) )
      {
         return false;
      }

      return true;
   }

   bool Texture2dDX11::Init( ID3D11Texture2D* texture )
   {
      if ( texture == nullptr || m_renderer == nullptr )
      {
         return false;
      }

      m_texture = texture;

      D3D11_TEXTURE2D_DESC desc;
      m_texture->GetDesc( &desc );
      
      m_width = desc.Width;
      m_height = desc.Height;

      if ( !InitSRV( desc ) )
      {
         return false;
      }

      return true;
   }

   bool Texture2dDX11::InitSRV( D3D11_TEXTURE2D_DESC desc )
   {
      auto device = m_renderer->GetDevice( );
      D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
      ZeroMemory( &srvDesc, sizeof( srvDesc ) );
      srvDesc.Texture2D.MipLevels = m_mipLevels;
      srvDesc.Texture2D.MostDetailedMip = 0;
      srvDesc.Format = desc.Format;
      srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

      auto result = device->CreateShaderResourceView( m_texture, &srvDesc, &m_srv );
      if ( FAILED( result ) )
      {
         return false;
      }

      return true;
   }

   bool Texture2dDX11::Bind( unsigned int startSlot, ShaderType shader )
   {
      if ( !m_bIsInitialized || m_renderer == nullptr || m_bIsBinded )
      {
         return false;
      }

      auto immediateContext = m_renderer->GetDeviceContext( );

      switch ( shader )
      {
      case ShaderType::VertexShader:
         immediateContext->VSSetShaderResources( startSlot, 1, &m_srv );
         break;
      case ShaderType::PixelShader:
         immediateContext->PSSetShaderResources( startSlot, 1, &m_srv );
         break;
      }

      m_bindedSlot = startSlot;
      m_bindedShader = shader;

      m_bIsBinded = true;
      return true;
   }

   void Texture2dDX11::Unbind( )
   {
      if ( !m_bIsBinded )
      {
         return;
      }

      auto immediateContext = m_renderer->GetDeviceContext( );
      switch ( m_bindedShader )
      {
      case ShaderType::VertexShader:
         immediateContext->VSSetShaderResources( m_bindedSlot, 1, nullptr );
         break;
      case ShaderType::PixelShader:
         immediateContext->PSSetShaderResources( m_bindedSlot, 1, nullptr );
         break;
      }

      m_bIsBinded = false;
   }
}