#pragma once

#include "ResourceDX11.h"

namespace Mile
{
   class MEAPI SamplerDX11
   {
   public:
      SamplerDX11( RendererDX11* renderer ) :
         m_sampler( nullptr ),
         m_bIsInit( false ),
         m_bindedSlot( 0 ),
         m_bIsBinded( false ),
         m_renderer( renderer )
      {
      }

      ~SamplerDX11( )
      {
         SafeRelease( m_sampler );
      }

      bool Init( D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressModeU, D3D11_TEXTURE_ADDRESS_MODE AddressModeV, D3D11_TEXTURE_ADDRESS_MODE AddressModeW, D3D11_COMPARISON_FUNC compFunc );
      bool Bind( unsigned int startSlot );
      void Unbind( );

      bool IsInitialized( ) const { return m_bIsInit; }

   private:
      RendererDX11*        m_renderer;
      ID3D11SamplerState*  m_sampler;
      bool                 m_bIsInit;
      unsigned int         m_bindedSlot;
      bool                 m_bIsBinded;

   };
}