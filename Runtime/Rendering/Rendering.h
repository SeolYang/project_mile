#pragma once

#include "Core/Helper.h"
#include "Core/SubSystem.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
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
      RDRT_Texture3D,
      RDRT_RenderTarget,
      RDRT_DepthStencilBuffer
   };

   enum class ShaderType
   {
      VertexShader,
      HullShader,
      DomainShader,
      GeometryShader,
      PixelShader
   };
}