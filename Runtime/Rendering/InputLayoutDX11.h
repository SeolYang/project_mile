#pragma once

#include "ResourceDX11.h"

namespace Mile
{
   class VertexShaderDX11;
   class MEAPI InputLayoutDX11
   {
      using InputLayoutElementList = std::vector<D3D11_INPUT_ELEMENT_DESC>;
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

      bool Init( InputLayoutElementList&& intputLayoutDescs, VertexShaderDX11* shader );
      bool Bind( );

      InputLayoutElementList GetElementDescs( ) const { return m_elementDescs; }

   private:
      RendererDX11*           m_renderer;

      ID3D11InputLayout*      m_inputLayout;
      bool                    m_bIsInitialized;

      InputLayoutElementList  m_elementDescs;

   };
}
