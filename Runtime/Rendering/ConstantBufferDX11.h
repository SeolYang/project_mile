#pragma once
#include "BufferDX11.h"
#include "ShaderDX11.h"

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
      
      virtual RenderResourceType GetResourceType( ) const override { return RenderResourceType::RDRT_ConstantBuffer; }

      virtual void* Map( ) override;
      virtual bool UnMap( ) override;

      bool Bind( unsigned int slot, ShaderType shaderType );

   };
}