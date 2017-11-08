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

      auto result = m_renderer->GetDevice( )->CreateTexture2D( &desc, &resource, &m_texture );
      if ( FAILED( result ) )
      {
         return false;
      }

      return true;
   }
}