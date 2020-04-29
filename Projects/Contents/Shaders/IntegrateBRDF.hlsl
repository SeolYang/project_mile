#include "BRDFs.hlsli"

/* Input & Output Structures */
struct VSInput
{
	float4 Position		: POSITION;
	float2 TexCoord		: TEXCOORDS0;
};

struct VSOutput
{
	float4 PositionSS	: SV_Position;
	float2 TexCoord		: TEXCOORD;
};

struct PSInput
{
	float4 ScreenPos	: SV_Position;
	float2 TexCoord		: TEXCOORD;
};

float2 IntegrateBRDF(float NdotV, float roughness)
{
   float3 V;
   V.x = sqrt(1.0f - (NdotV * NdotV));
   V.y = 0.0f;
   V.z = NdotV;

   float A = 0.0f;
   float B = 0.0f;

   float3 N = float3(0.0f, 0.0f, 1.0f);
   const uint SAMPLE_COUNT = 1024u;
   for (uint idx = 0; idx < SAMPLE_COUNT; ++idx)
   {
      float2 Xi = Hammersley(idx, SAMPLE_COUNT);
      float3 H = ImportanceSampleGGX(Xi, N, roughness);
      float3 L = normalize((2.0f * dot(V, H) * H) - V);

      float NdotL = max(L.z, 0.0f);
      float NdotH = max(H.z, 0.0f);
      float VdotH = max(dot(V, H), 0.0f);

      if (NdotL > 0.0f)
      {
         float G = GeometrySmith(N, V, L, roughness);
         float G_Vis = (G * VdotH) / (NdotH * NdotV);
         float Fc = pow(1.0f - VdotH, 5.0f);

         A += (1.0f - Fc) * G_Vis;
         B += Fc * G_Vis;
      }
   }

   A /= float(SAMPLE_COUNT);
   B /= float(SAMPLE_COUNT);
   return float2(A, B);
}

VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionSS = input.Position;
	output.TexCoord = input.TexCoord;
	return output;
}

float2 MilePS(in PSInput input) : SV_Target0
{
	float2 integratedBRDF = IntegrateBRDF(input.TexCoord.x, input.TexCoord.y);
   return integratedBRDF;
}