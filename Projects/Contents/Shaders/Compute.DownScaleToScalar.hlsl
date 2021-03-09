#include "DownScale.hlsli"

#define MAX_GROUPS 512 // For 4K Resolution ( 3840 * 2160 / (16*1024) ) = 506.25

StructuredBuffer<float> AvgValues1D : register(t0);
RWStructuredBuffer<float> AvgLuminance : register(u0);
RWStructuredBuffer<float> PrevAvgLum : register(u1);

groupshared float SharedAvgFinal[MAX_GROUPS];

[numthreads(MAX_GROUPS, 1, 1)]
void MileCS(uint3 groupID : SV_GroupID, uint3 dispatchThreadID : SV_DispatchThreadID, uint3 groupThreadID : SV_GroupThreadID)
{
   float avgLuminance = 0.0;
   if (dispatchThreadID.x < GroupSize)
   {
      avgLuminance = AvgValues1D[dispatchThreadID.x];
   }
   SharedAvgFinal[dispatchThreadID.x] = avgLuminance;

   GroupMemoryBarrierWithGroupSync();

   // 512 -> 64 downscale
   if (dispatchThreadID.x % 8 == 0)
   {
      float stepAvgLuminance = avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 1) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 1] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 2) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 2] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 3) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 3] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 4) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 4] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 5) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 5] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 6) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 6] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 7) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 7] : avgLuminance;

      avgLuminance = stepAvgLuminance;
      SharedAvgFinal[dispatchThreadID.x] = stepAvgLuminance;
   }

   GroupMemoryBarrierWithGroupSync();

   // 64 -> 8 downscale
   if (dispatchThreadID.x % 64 == 0)
   {
      float stepAvgLuminance = avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 8) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 8] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 16) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 16] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 24) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 24] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 32) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 32] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 40) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 40] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 48) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 48] : avgLuminance;
      stepAvgLuminance += (dispatchThreadID.x + 56) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 56] : avgLuminance;

      avgLuminance = stepAvgLuminance;
      SharedAvgFinal[dispatchThreadID.x] = stepAvgLuminance;
   }

   GroupMemoryBarrierWithGroupSync();

   // 8 -> 1 downscale
   if (dispatchThreadID.x == 0)
   {
      float finalLumValue = avgLuminance;
      finalLumValue += (dispatchThreadID.x + 64) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 64] : avgLuminance;
      finalLumValue += (dispatchThreadID.x + 128) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 128] : avgLuminance;
      finalLumValue += (dispatchThreadID.x + 192) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 192] : avgLuminance;
      finalLumValue += (dispatchThreadID.x + 256) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 256] : avgLuminance;
      finalLumValue += (dispatchThreadID.x + 320) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 320] : avgLuminance;
      finalLumValue += (dispatchThreadID.x + 384) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 384] : avgLuminance;
      finalLumValue += (dispatchThreadID.x + 448) < GroupSize ? SharedAvgFinal[dispatchThreadID.x + 448] : avgLuminance;

      finalLumValue /= 512.0f;

      float adaptedAvgLum = finalLumValue;
      if (PrevAvgLum[0] > 0.0)
      {
         adaptedAvgLum = lerp(PrevAvgLum[0], finalLumValue, (PrevAvgLum[0] < finalLumValue ? LightAdaption : DarkAdaption));
      }

      AvgLuminance[0] = adaptedAvgLum;
      PrevAvgLum[0] = adaptedAvgLum;
   }
}