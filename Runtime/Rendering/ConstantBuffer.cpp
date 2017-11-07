#include "ConstantBufferDX11.h"
#include "RendererDX11.h"

namespace Mile
{
   bool ConstantBufferDX11::Init( unsigned int size )
   {
      if ( m_bIsInitialized
           || ( m_renderer == nullptr )
           || ( size == 0 ) )
      {
         return false;
      }

      D3D11_BUFFER_DESC desc;
      ZeroMemory( &desc, sizeof( desc ) );
      desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      desc.ByteWidth = size;
      desc.StructureByteStride = 0;
      desc.Usage = D3D11_USAGE_DYNAMIC;
      desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      desc.MiscFlags = 0;

      auto result = m_renderer->GetDevice( )->CreateBuffer( &desc, nullptr, &m_buffer );
      if ( FAILED( result ) )
      {
         return false;
      }

      m_desc = desc;
      m_bIsInitialized = true;
      return true;
   }
}