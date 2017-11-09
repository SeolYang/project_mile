#pragma once

#include "ResourceDX11.h"

namespace Mile
{
   class VertexShaderDX11;
   class MEAPI InputLayoutDX11
   {
   public:
      InputLayoutDX11( RendererDX11* renderer ) : 
         m_renderer( renderer ),
         m_inputLayout( nullptr ),
         m_bIsInitialized( false )
      {
      }

      ~InputLayoutDX11( )
      {
         SafeRelease( m_inputLayout );
      }

      bool Init( const std::vector<D3D11_INPUT_ELEMENT_DESC>& intputLayoutDescs, VertexShaderDX11* shader );
      bool Bind( );

   private:
      RendererDX11*           m_renderer;

      ID3D11InputLayout*      m_inputLayout;
      bool                    m_bIsInitialized;

   };
}
