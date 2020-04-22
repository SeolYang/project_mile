#ifndef __COMMON_INCLUDED__
#define __COMMON_INCLUDED__

static const float PI = 3.14159265359;

float3x3 float4x4_to_float3x3(float4x4 m)
{
   float3x3 output;
   output[0][0] = m[0][0];
   output[0][1] = m[0][1];
   output[0][2] = m[0][2];

   output[1][0] = m[1][0];
   output[1][1] = m[1][1];
   output[1][2] = m[1][2];

   output[2][0] = m[2][0];
   output[2][1] = m[2][1];
   output[2][2] = m[2][2];

   output[3][0] = m[3][0];
   output[3][1] = m[3][1];
   output[3][2] = m[3][2];
   return output;
}

float4x4 float3x3_to_float4x4(float3x3 m)
{
   float4x4 output;
   output[0][0] = m[0][0];
   output[0][1] = m[0][1];
   output[0][2] = m[0][2];
   output[0][3] = 0.0f;

   output[1][0] = m[1][0];
   output[1][1] = m[1][1];
   output[1][2] = m[1][2];
   output[1][3] = 0.0f;

   output[2][0] = m[2][0];
   output[2][1] = m[2][1];
   output[2][2] = m[2][2];
   output[2][3] = 0.0f;

   output[3][0] = m[3][0];
   output[3][1] = m[3][1];
   output[3][2] = m[3][2];
   output[3][3] = 1.0f;
   return output;
}

float4x4 remove_translation(float4x4 m)
{
   return float3x3_to_float4x4(float4x4_to_float3x3(m));
}

#endif