#ifndef __COMMON_INCLUDED__
#define __COMMON_INCLUDED__

static const float PI = 3.14159265359;

float4x4 remove_translation(float4x4 m)
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

   output[3][0] = 0.0f;
   output[3][1] = 0.0f;
   output[3][2] = 0.0f;
   output[3][3] = 1.0f;

   return output;
}

float RadicalInverse_VanDerCorpus(uint bits)
{
   bits = (bits << 16u) | (bits >> 16u);
   bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
   bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
   bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
   bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
   return float(bits) * 2.3283064365386963e-10;
}

float2 Hammersley(uint idx, uint N)
{
   return float2(
      (float(idx) / float(N)),
      RadicalInverse_VanDerCorpus(idx));
}

#endif