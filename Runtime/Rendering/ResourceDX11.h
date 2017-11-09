#pragma once

#include "RendererDX11.h"

namespace Mile
{
   enum class RenderResourceType
   {
      RDRT_VertexBuffer,
      RDRT_IndexBuffer,
      RDRT_ConstantBuffer,
      RDRT_StructuredBuffer,
      RDRT_ByteAddressBuffer,
      RDRT_IndirectArgumentsBuffer,
      RDRT_Texture1D,
      RDRT_Texture2D,
      RDRT_Texture3D,
      RDRT_RenderTarget,
      RDRT_DepthStencilBuffer
   };

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