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
      
      virtual void* Map( ) override;
      virtual bool UnMap( ) override;

      bool BindAtShader( unsigned int slot, RenderResourceType shaderType );

      virtual RenderResourceType GetResourceType( ) const override { return RenderResourceType::RDRT_ConstantBuffer; }

   };
}