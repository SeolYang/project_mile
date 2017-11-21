#include "SamplerDX11.h"

namespace Mile
{
   bool SamplerDX11::Init( D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressModeU, D3D11_TEXTURE_ADDRESS_MODE AddressModeV, D3D11_TEXTURE_ADDRESS_MODE AddressModeW, D3D11_COMPARISON_FUNC compFunc )
   {
      if ( m_bIsInit || m_renderer == nullptr )
      {
         return false;
      }

      D3D11_SAMPLER_DESC desc;
      ZeroMemory( &desc, sizeof( desc ) );
      desc.AddressU = AddressModeU;
      desc.AddressV = AddressModeV;
      desc.AddressW = AddressModeW;
      desc.Filter = filter;
      desc.ComparisonFunc = compFunc;
      desc.MipLODBias = 0.0f;
      desc.BorderColor[ 0 ] = 0.0f;
      desc.BorderColor[ 1 ] = 0.0f;
      desc.BorderColor[ 2 ] = 0.0f;
      desc.BorderColor[ 3 ] = 0.0f;
      desc.MinLOD = FLT_MIN;
      desc.MaxLOD = FLT_MAX;

      auto result = m_renderer->GetDevice( )->CreateSamplerState( &desc, &m_sampler );
      if ( FAILED( result ) )
      {
         return false;
      }

      return true;
   }

   bool SamplerDX11::Bind( unsigned int startSlot )
   {
      if ( !m_bIsInit || m_renderer == nullptr )
      {
         return false;
      }

      m_renderer->GetDeviceContext( )->PSSetSamplers( startSlot, 1, &m_sampler );
      return true;
   }
}