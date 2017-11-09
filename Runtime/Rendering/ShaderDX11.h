#pragma once

#include "ResourceDX11.h"

namespace Mile
{
   class MEAPI ShaderDX11
   {
   public:
      ShaderDX11( RendererDX11* renderer ) :
         m_renderer( renderer ),
         m_blob( nullptr ),
         m_errorBlob( nullptr ),
         m_bIsCompiled( false )
      {
      }

      ~ShaderDX11( )
      {
         SafeRelease( m_blob );
         SafeRelease( m_errorBlob );
      }

      virtual bool Init( const String& shaderPath ) = 0;

      ID3D10Blob* GetBlob( ) { return m_blob; }

      bool IsCompiled( ) const { return m_bIsCompiled; }
      virtual ShaderType GetShaderType( ) const = 0;

   protected:
      virtual bool Compile( const String& shaderPath, ShaderType shaderType ) final
      {
         if ( !m_bIsCompiled || m_renderer == nullptr )
         {
            return false;
         }

         std::string entryPoint = "Mile";
         std::string target = "_5_0";

         switch ( shaderType )
         {
         case ShaderType::VertexShader:
            entryPoint += "VS";
            target = "vs" + target;
            break;
         case ShaderType::HullShader:
            entryPoint += "HS";
            target = "hs" + target;
            break;
         case ShaderType::DomainShader:
            entryPoint += "DS";
            target = "ds" + target;
            break;
         case ShaderType::GeometryShader:
            entryPoint += "GS";
            target = "gs" + target;
            break;
         case ShaderType::PixelShader:
            entryPoint += "PS";
            target = "ps" + target;
            break;
         }

         auto compileFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_OPTIMIZATION_LEVEL3;

#if defined(_DEBUG) | defined(DEBUG)
         compileFlags |= D3D10_SHADER_DEBUG;
#endif

         auto result = D3DCompileFromFile( shaderPath.c_str( ),
                             nullptr,
                             D3D_COMPILE_STANDARD_FILE_INCLUDE,
                             entryPoint.c_str( ),
                             target.c_str( ),
                             compileFlags, 0,
                             &m_blob,
                             &m_errorBlob );

         if ( FAILED( result ) )
         {
            /*@TODO: logging shader compile error **/
            return false;
         }

         return true;
      }

   protected:
      RendererDX11*     m_renderer;

      ID3D10Blob*       m_blob;
      ID3D10Blob*       m_errorBlob;

      bool              m_bIsCompiled;

   };
}