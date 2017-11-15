#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Mile
{
   /*
   * @ Vector4(float4) : POSITION
   **/
   struct MEAPI VertexP
   {
      Vector4 Position;
   };

   /*
   * @ Vector4(float4) : POSITION
   * @ Vector2(float2) : TEXCOORD0
   * @ Vector3(float3) : NORMAL
   **/
   struct MEAPI VertexPTN
   {
      Vector4 Position;
      Vector2 TexCoord;
      Vector3 Normal;
   };

   /*
   * @ Vector4(float4) : POSITION
   * @ Vector2(float2) : TEXCOORD0
   * @ Vector3(float3) : NORMAL
   * @ Vector4(float4) : TANGENT
   **/
   struct MEAPI VertexPTNTAN
   {
      Vector4 Position;
      Vector2 TexCoord;
      Vector3 Normal;
      Vector4 Tangent;
   };
}
