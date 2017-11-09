#pragma once

#include "ShaderDX11.h"

namespace Mile
{
   class InputLayoutDX11;
   class MEAPI VertexShaderDX11 : public ShaderDX11
   {
      using InputLayoutPtr = std::shared_ptr<InputLayoutDX11>;

   public:
      VertexShaderDX11( RendererDX11* renderer ) :
         m_shader( nullptr ),
         m_inputLayout( nullptr ),
         ShaderDX11( renderer )
      {
      }

      ~VertexShaderDX11( )
      {
         SafeRelease( m_shader );
      }

      virtual bool Init( const String& shaderPath ) override;
      virtual bool Bind( ) override;

      virtual ShaderType GetShaderType( ) const override { return ShaderType::VertexShader; }

   private:
      std::vector<D3D11_INPUT_ELEMENT_DESC>&& Reflect( ) const;

   private:
      ID3D11VertexShader*  m_shader;
      InputLayoutPtr       m_inputLayout;

   };
}