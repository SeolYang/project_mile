#include "IndexBufferDX11.h"
#include "RendererDX11.h"

namespace Mile
{
   bool IndexBufferDX11::Init( const std::vector<unsigned int>& indicies )
   {
      if ( m_bIsInitialized || ( m_renderer == nullptr ) )
      {
         return false;
      }

      D3D11_BUFFER_DESC desc;

      ZeroMemory( &desc, sizeof( desc ) );
      desc.ByteWidth = static_cast< unsigned int >( sizeof( unsigned int ) * indicies.size( ) );
      desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      desc.StructureByteStride = 0;
      desc.MiscFlags = 0;
      desc.Usage = D3D11_USAGE_IMMUTABLE;
      desc.CPUAccessFlags = 0;

      D3D11_SUBRESOURCE_DATA subResource;
      ZeroMemory( &subResource, sizeof( subResource ) );
      subResource.pSysMem = indicies.data( );

      auto result = m_renderer->GetDevice( )->CreateBuffer( &desc, &subResource, &m_buffer );
      if ( FAILED( result ) )
      {
         return false;
      }

      m_desc = desc;
      m_bIsInitialized = true;
      return true;
   }

   bool IndexBufferDX11::Bind( )
   {
      if ( !m_bIsInitialized )
      {
         return false;
      }

      auto deviceContext = m_renderer->GetDeviceContext( );
      deviceContext->IASetIndexBuffer( m_buffer, DXGI_FORMAT_R32_UINT, 0 );

      return true;
   }
}