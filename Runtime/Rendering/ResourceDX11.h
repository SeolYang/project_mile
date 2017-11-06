#pragma once

#include "Core\Helper.h"
#include <d3d11.h>

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
      RDRT_Texture3D
   };

   class MEAPI ResourceDX11
   {
   public:
      ResourceDX11( ) : m_bIsInitialized( false )
      {
      }

      virtual ~ResourceDX11( )
      {
      }

      virtual ID3D11Resource* GetResource( ) = 0;
      virtual RenderResourceType GetResourceType( ) = 0;
      
      bool IsInitialized( ) const { return m_bIsInitialized; }

   protected:
      bool     m_bIsInitialized;

   };
}