#include "Common.hlsli"

cbuffer DownScaleConstants : register (b0)
{
   uint2 Resolution : packoffset(c0);
   uint Domain : packoffset(c0.z);
   uint GroupSize : packoffset(c0.w);
   float LightAdaption : packoffset(c1);
   float DarkAdaption : packoffset(c1.y);
   float MinBrightness : packoffset(c1.z);
   float MaxBrightness : packoffset(c1.w);
}

static const float4 LUMINANCE_FACTOR = float4(0.299, 0.597, 0.114, 0.0);