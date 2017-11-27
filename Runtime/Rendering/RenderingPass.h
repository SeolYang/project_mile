#pragma once

#include "RendererDX11.h"

namespace Mile
{
   class VertexShaderDX11;
   class PixelShaderDX11;
   class MEAPI RenderingPass
   {
   public:
      RenderingPass( RendererDX11* renderer ) :
         m_renderer( renderer )
      {
      }

      ~RenderingPass( )
      {
         SafeDelete( m_vertexShader );
         SafeDelete( m_pixelShader );
      }

      bool Init( const String& vertexShader,
                 const String& pixelShader = TEXT( "null" ),
                 const String& geometryShader = TEXT( "null" ),
                 const String& hullShader = TEXT( "null" ),
                 const String& domainShader = TEXT( "null" ) );

      bool Bind( );
      void Unbind( );
      
   private:
      bool InitVS( const String& filePath );
      bool InitPS( const String& filePath );

   private:
      RendererDX11*     m_renderer;
      VertexShaderDX11* m_vertexShader;
      PixelShaderDX11*  m_pixelShader;

   };
}