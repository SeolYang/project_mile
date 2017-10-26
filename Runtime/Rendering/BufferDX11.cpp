#include "BufferDX11.h"

namespace Mile
{
   ID3D11Resource* BufferDX11::GetResource( )
   {
      return m_buffer;
   }
}
