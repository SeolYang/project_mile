#include "PixelShaderDX11.h"
#include "SamplerDX11.h"

namespace Mile
{
   bool PixelShaderDX11::Init( const String& shaderPath )
   {
      if ( m_bIsCompiled || m_renderer == nullptr )
      {
         return false;
      }

      if ( !Compile( shaderPath, ShaderType::PixelShader ) )
      {
         return false;
      }

      auto result = m_renderer->GetDevice( )->CreatePixelShader( m_blob->GetBufferPointer( ),
                                                                 m_blob->GetBufferSize( ),
                                                                 nullptr,
                                                                 &m_shader );

      if ( FAILED( result ) )
      {
         return false;
      }

      return true;
   }

   bool PixelShaderDX11::Bind( )
   {
      if ( !m_bIsCompiled || m_renderer == nullptr )
      {
         return false;
      }

      m_renderer->GetDeviceContext( )->PSSetShader( m_shader,
                                                    nullptr,
                                                    0 );

      for ( int idx = 0; idx < m_samplers.size( ); ++idx )
      {
         if ( !m_samplers[ idx ]->Bind( idx ) )
         {
            return false;
         }
      }

      return true;
   }

   void PixelShaderDX11::Unbind( )
   {
      if ( m_renderer != nullptr )
      {
         auto deviceContext = m_renderer->GetDeviceContext( );
         deviceContext->PSSetShader( nullptr, nullptr, 0 );
      }
   }

   bool PixelShaderDX11::AddSampler( D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressModeU, D3D11_TEXTURE_ADDRESS_MODE AddressModeV, D3D11_TEXTURE_ADDRESS_MODE AddressModeW, D3D11_COMPARISON_FUNC compFunc )
   {
      SamplerDX11* sampler = new SamplerDX11( m_renderer );
      if ( !sampler->Init( filter, AddressModeU, AddressModeV, AddressModeW, compFunc ) )
      {
         SafeDelete( sampler );
         return false;
      }

      this->m_samplers.push_back( sampler );
      return true;
   }

   bool PixelShaderDX11::AddSampler( D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressMode, D3D11_COMPARISON_FUNC compFunc )
   {
      return this->AddSampler( filter, AddressMode, AddressMode, AddressMode, compFunc );
   }
}