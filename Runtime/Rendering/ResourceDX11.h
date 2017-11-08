#pragma once

#include "RendererDX11.h"

namespace Mile
{
   class RendererDX11;
   class MEAPI ResourceDX11
   {
   public:
      ResourceDX11( RendererDX11* renderer ) : 
         m_bIsInitialized( false ),
         m_renderer( renderer )
      {
      }

      virtual ~ResourceDX11( )
      {
      }

      virtual ID3D11Resource* GetResource( ) = 0;
      virtual RenderResourceType GetResourceType( ) const = 0;
      
      bool IsInitialized( ) const { return m_bIsInitialized; }

   protected:
      RendererDX11*  m_renderer;
      bool           m_bIsInitialized;

   };
}