#pragma once
#include "BufferDX11.h"

namespace Mile
{
   class MEAPI IndexBufferDX11 : public BufferDX11
   {
   public:
      IndexBufferDX11( ) { }

      bool Init( const std::vector<unsigned int>& indicies, bool dynamic = false );
      
   };
}