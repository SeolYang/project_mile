#include "RenderingPass.h"
#include "VertexShaderDX11.h"
#include "PixelShaderDX11.h"

namespace Mile
{
   bool RenderingPass::Init( const String& vertexShader,
                             const String& pixelShader,
                             const String& geometryShader,
                             const String& hullShader,
                             const String& domainShader )
   {
      bool bIsInitialized = m_vertexShader != nullptr || m_pixelShader != nullptr;
      if ( m_renderer == nullptr || bIsInitialized )
      {
         return false;
      }

      if ( !InitVS( vertexShader ) )
      {
         return false;
      }

      if ( !InitPS( pixelShader ) )
      {
         return false;
      }

      return true;
   }

   bool RenderingPass::Bind( )
   {
      if ( m_renderer == nullptr )
      {
         return false;
      }

      if ( m_vertexShader != nullptr )
      {
         m_vertexShader->Bind( );
      }

      if ( m_pixelShader != nullptr )
      {
         m_pixelShader->Bind( );
      }

      return true;
   }

   void RenderingPass::Unbind( )
   {
      if ( m_renderer != nullptr )
      {
         if ( m_vertexShader != nullptr )
         {
            m_vertexShader->Unbind( );
         }
         
         if ( m_pixelShader != nullptr )
         {
            m_pixelShader->Unbind( );
         }
      }
   }

   bool RenderingPass::InitVS( const String& filePath )
   {
      if ( m_vertexShader != nullptr )
      {
         return false;
      }

      if ( filePath == TEXT( "null" ) )
      {
         return true;
      }

      m_vertexShader = new VertexShaderDX11( m_renderer );
      return m_vertexShader->Init( filePath );
   }

   bool RenderingPass::InitPS( const String& filePath )
   {
      if ( m_pixelShader != nullptr )
      {
         return false;
      }

      if ( filePath == TEXT( "null" ) )
      {
         return true;
      }

      m_pixelShader = new PixelShaderDX11( m_renderer );
      return m_pixelShader->Init( filePath );
   }

}