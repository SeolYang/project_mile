#include "PixelShaderDX11.h"

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

      return true;
   }
}