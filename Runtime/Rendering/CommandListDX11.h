#pragma once

#include "Rendering.h"

namespace Mile
{
   class RendererDX11;
   // Caching CommandList
   class MEAPI CommandListDX11
   {
   public:
      CommandListDX11( RendererDX11* renderer );
      ~CommandListDX11( );

      bool Init( ID3D11DeviceContext* deviceContext );
      void DeInit( );
      bool Execute( ID3D11DeviceContext* deviceContext, bool restoreContextState );

      ID3D11CommandList* GetCommandList( ) const { return m_list; }
      bool IsInitialized( ) const { return m_bIsInitialized; }

   private:
      RendererDX11*           m_renderer;
      bool                    m_bIsInitialized;
      ID3D11CommandList*      m_list;

   };
}