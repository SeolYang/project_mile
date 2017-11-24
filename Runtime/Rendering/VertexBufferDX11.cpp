#include "VertexBufferDX11.h"

namespace Mile
{
   bool VertexBufferDX11::Bind( unsigned int startSlot )
   {
      if ( !m_bIsInitialized )
      {
         return false;
      }

      auto immediateContext = m_renderer->GetDeviceContext( );

      unsigned int offset = 0;
      immediateContext->IASetVertexBuffers( startSlot, 1, &m_buffer, &m_stride, &offset );

      return true;
   }
}