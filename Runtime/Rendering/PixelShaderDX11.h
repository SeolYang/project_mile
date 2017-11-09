#pragma once

#include "ShaderDX11.h"

namespace Mile
{
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
         SafeRelease( m_shader );
      }

      virtual bool Init( const String& shaderPath ) override;
      virtual bool Bind( ) override;

      virtual ShaderType GetShaderType( ) const override { return ShaderType::PixelShader; }

   private:
      ID3D11PixelShader*   m_shader;

   };
}
