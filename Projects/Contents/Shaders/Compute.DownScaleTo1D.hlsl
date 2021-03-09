#include "DownScale.hlsli"

Texture2D HDRInput : register(t0);
RWStructuredBuffer<float> AvgLuminance : register(u0);

groupshared float SharedPositions[1024];

float DownScale4x4(uint2 currentPixel, uint groupThreadID)
{
   float avgLuminance = 0.0;
   if (currentPixel.y < Resolution.y)
   {
      int3 fullResPos = int3(currentPixel * 4, 0);
      float4 downScaled = float4(0.0, 0.0, 0.0, 0.0);

      [unroll]
      for (int yy = 0; yy < 4; ++yy)
      {
         [unroll]
         for (int xx = 0; xx < 4; ++xx)
         {
            downScaled += HDRInput.Load(fullResPos, int2(xx, yy));
         }
      }

      downScaled /= 16.0;

      avgLuminance = dot(downScaled, LUMINANCE_FACTOR);
      SharedPositions[groupThreadID] = avgLuminance;
   }

   GroupMemoryBarrierWithGroupSync();
   return avgLuminance;
}

float DownScale1024to4(uint dispatchThreadID, uint groupThreadID, float avgLuminance)
{
   [unroll]
   for (uint groupSize = 4, step1 = 1, step2 = 2, step3 = 3; groupSize < 1024; groupSize *= 4, step1 *= 4, step2 *= 4, step3 *= 4)
   {
      if (groupThreadID % groupSize == 0)
      {
         float stepAvgLuminance = avgLuminance;
         stepAvgLuminance += (dispatchThreadID + step1) < Domain ? SharedPositions[groupThreadID + step1] : avgLuminance;
         stepAvgLuminance += (dispatchThreadID + step2) < Domain ? SharedPositions[groupThreadID + step2] : avgLuminance;
         stepAvgLuminance += (dispatchThreadID + step3) < Domain ? SharedPositions[groupThreadID + step3] : avgLuminance;

         avgLuminance = stepAvgLuminance;
         SharedPositions[groupThreadID] = stepAvgLuminance;
      }

      GroupMemoryBarrierWithGroupSync();
   }

   return avgLuminance;
}

void DownScale4to1(uint dispatchThreadID, uint groupThreadID, uint groupID, float avgLuminance)
{
   if (groupThreadID == 0)
   {
      float finalAvgLuminance = avgLuminance;
      finalAvgLuminance += (dispatchThreadID + 256) < Domain ? SharedPositions[groupThreadID + 256] : avgLuminance;
      finalAvgLuminance += (dispatchThreadID + 512) < Domain ? SharedPositions[groupThreadID + 512] : avgLuminance;
      finalAvgLuminance += (dispatchThreadID + 768) < Domain ? SharedPositions[groupThreadID + 768] : avgLuminance;
      finalAvgLuminance /= 1024.0;

      AvgLuminance[groupID] = finalAvgLuminance;
   }
}

[numthreads(1024, 1, 1)]
void MileCS(uint3 groupID : SV_GroupID, uint3 dispatchThreadID : SV_DispatchThreadID, uint3 groupThreadID : SV_GroupThreadID)
{
   uint2 currentPixel = uint2(dispatchThreadID.x % Resolution.x, dispatchThreadID.x / Resolution.x);

   // 16픽셀 그룹 -> 하나의 픽셀로 downscale (1024*16픽셀 처리)
   float avgLuminance = DownScale4x4(currentPixel, groupThreadID.x);

   // 1024개의 줄여진 픽셀 -> 4개의 값으로 downscale
   avgLuminance = DownScale1024to4(dispatchThreadID.x, groupThreadID.x, avgLuminance);

   // 4개의 값 -> 1개의 값으로 down scale
   DownScale4to1(dispatchThreadID.x, groupThreadID.x, groupID.x, avgLuminance);
}