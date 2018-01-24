#pragma once
#include "BufferDX11.h"

namespace Mile
{
   class MEAPI IndexBufferDX11 : public BufferDX11
   {
   public:
      IndexBufferDX11( RendererDX11* renderer ) :
         BufferDX11( renderer )
      { 
      }

      bool Init( const std::vector<unsigned int>& indicies );
      bool Bind( ID3D11DeviceContext& deviceContext );

      virtual RenderResourceType GetResourceType( ) const override { return RenderResourceType::RDRT_IndexBuffer; }
      
   };
}