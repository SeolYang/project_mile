#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Mile
{
   /*
   * @ Vector4(float4) : POSITION
   **/
   struct MEAPI alignas( 16 ) VertexPos
   {
      Vector4 Position;
   };

   /*
   * @ Vector4(float4) : POSITION
   * @ Vector2(float2) : TEXCOORD0
   **/
   struct MEAPI alignas( 16 ) VertexPosTex
   {
      Vector4 Position;
      Vector2 TexCoord;
   };

   /*
   * @ Vector4(float4) : POSITION
   * @ Vector2(float2) : TEXCOORD0
   * @ Vector3(float3) : NORMAL
   **/
   struct MEAPI alignas( 16 ) VertexPosTexNormal
   {
      Vector4 Position;
      Vector2 TexCoord;
      Vector3 Normal;
   };

   using VertexPTN = VertexPosTexNormal;

   /*
   * @ Vector4(float4) : POSITION
   * @ Vector2(float2) : TEXCOORD0
   * @ Vector3(float3) : NORMAL
   * @ Vector4(float4) : TANGENT
   **/
   struct MEAPI alignas( 16 ) VertexPosTexNT
   {
      Vector4 Position;
      Vector2 TexCoord;
      Vector3 Normal;
      Vector4 Tangent;
   };

   /*
   * @ Vector4(float4) : POSITION
   * @ Vector2(float2) : TEXCOORD0
   * @ Vector3(float3) : NORMAL
   * @ Vector4(float4) : TANGENT
   * @ Vector4(float4) : BITANGENT
   **/
   struct MEAPI alignas( 16 ) VertexPosTexNTB
   {
      Vector4 Position;
      Vector2 TexCoord;
      Vector3 Normal;
      Vector4 Tangent;
      Vector4 BiTangent;
   };

}
