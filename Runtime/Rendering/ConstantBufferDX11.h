#pragma once
#include "BufferDX11.h"

namespace Mile
{
   class MEAPI ConstantBufferDX11 : public BufferDX11
   {
   public:
      ConstantBufferDX11( RendererDX11* renderer ) :
         BufferDX11( renderer )
      {
      }

      ~ConstantBufferDX11( )
      {
         this->UnMap( );
      }

      bool Init( unsigned int size );

   };
}