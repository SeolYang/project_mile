#pragma once
#include "Resource/Resource.h"

namespace Mile
{
   enum class DXResourceType
   {
      DXRT_VertexBuffer,
      DXRT_IndexBuffer,
      DXRT_ConstantBuffer,
      DXRT_StructuredBuffer,
      DXRT_ByteAddressBuffer,
      DXRT_IndirectArgumentsBuffer,
      DXRT_Texture1D,
      DXRT_Texture2D,
      DXRT_Texture3D
   };

   class ResourceDX11
   {
   public:
      ResourceDX11( );

      virtual class ID3D11Resource* GetResource( ) = 0;
      virtual DXResourceType GetResourceType( ) = 0;
   };
}