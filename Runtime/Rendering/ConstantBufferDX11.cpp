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

   void* ConstantBufferDX11::Map( )
   {
      if ( !m_bIsInitialized || m_renderer == nullptr )
      {
         return nullptr;
      }

      D3D11_MAPPED_SUBRESOURCE resource;
      auto result = m_renderer->GetDeviceContext( )->Map(
         m_buffer, 
         0, D3D11_MAP_WRITE_DISCARD, 0,
         &resource );

      if ( FAILED( result ) )
      {
         return nullptr;
      }

      return resource.pData;
   }

   bool ConstantBufferDX11::UnMap( )
   {
      if ( !m_bIsInitialized || m_renderer == nullptr )
      {
         return false;
      }

      m_renderer->GetDeviceContext( )->Unmap( m_buffer, 0 );
      return true;
   }

   bool ConstantBufferDX11::Bind( unsigned int startSlot, ShaderType shaderType )
   {
      if ( !m_bIsInitialized || m_renderer == nullptr )
      {
         return false;
      }

      auto deviceContext = m_renderer->GetDeviceContext( );
      switch ( shaderType )
      {
      case ShaderType::VertexShader:
         deviceContext->VSSetConstantBuffers( startSlot, 1, &m_buffer );
         break;
      case ShaderType::HullShader:
         deviceContext->HSSetConstantBuffers( startSlot, 1, &m_buffer );
         break;
      case ShaderType::DomainShader:
         deviceContext->DSSetConstantBuffers( startSlot, 1, &m_buffer );
         break;
      case ShaderType::GeometryShader:
         deviceContext->GSSetConstantBuffers( startSlot, 1, &m_buffer );
         break;
      case ShaderType::PixelShader:
         deviceContext->PSSetConstantBuffers( startSlot, 1, &m_buffer );
         break;
      default:
         return false;
      }

      return true;
   }
}