#pragma once

#include "ShaderDX11.h"

namespace Mile
{
   class SamplerDX11;
   class MEAPI PixelShaderDX11 : public ShaderDX11
   {
   public:
      PixelShaderDX11( RendererDX11* renderer ) :
         m_shader( nullptr ),
         ShaderDX11( renderer )
      {
      }

      ~PixelShaderDX11( )
      {
         for ( auto sampler : m_samplers )
         {
            SafeDelete( sampler );
         }

         SafeRelease( m_shader );
      }

      virtual bool Init( const String& shaderPath ) override;
      virtual bool Bind( ) override;
      virtual void Unbind( ) override;

      virtual ShaderType GetShaderType( ) const override { return ShaderType::PixelShader; }

      bool AddSampler( D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressModeU, D3D11_TEXTURE_ADDRESS_MODE AddressModeV, D3D11_TEXTURE_ADDRESS_MODE AddressModeW, D3D11_COMPARISON_FUNC compFunc );
      bool AddSampler( D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressMode, D3D11_COMPARISON_FUNC compFunc );

   private:
      ID3D11PixelShader*   m_shader;
      std::vector<SamplerDX11*> m_samplers;
   };
}
