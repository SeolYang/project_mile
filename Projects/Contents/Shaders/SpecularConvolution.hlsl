#include "BRDFs.hlsli"

/* Shader In/output Structures **/
struct VSInput
{
	float4 Position : POSITION;
};

struct VSOutput
{
	float4 Position	: SV_Position; // Clip Space
	float3 PositionWS : POSITIONWS; // Local Space
};

struct PSInput
{
	float4 Position	: SV_Position; // Screen Space
	float3 PositionWS : POSITIONWS;
};

/* Constant Buffers (Vertex Shader) **/
cbuffer TransfromBuffer
{
	float4x4 ViewProj;
};

/* Constant Buffers (Pixel Shader) **/
cbuffer PrefilteringParams
{
	float Roughness : packoffset(c0);
	float EnvironmentMapSize : packoffset(c0.y);
}

/* Textures & Samplers */
TextureCube EnvironmentMap			: register(t0);
SamplerState LinearBorderSampler : register(s0);

/* Shader Programs */
VSOutput MileVS(in VSInput input)
{
	VSOutput output;

	output.Position = mul(ViewProj, input.Position);
	output.PositionWS = input.Position.xyz;

	return output;
}

float4 MilePS(in PSInput input) : SV_Target0
{
	const uint SAMPLE_COUNT = 64u;
	float3 N = normalize(input.PositionWS);
	float3 R = N;
	float3 V = R;

	float totalWeight = 0.0f;
	float3 prefilteredColor = float3(0.0f, 0.0f, 0.0f);
	for (unsigned int idx = 0u; idx < SAMPLE_COUNT; ++idx)
	{
		float2 Xi = Hammersley(idx, SAMPLE_COUNT);
		float3 H = ImportanceSampleGGX(Xi, N, Roughness);
		float3 L = normalize((2.0f * dot(V, H) * H) - V); // Reflect vector

		float NdotL = max(dot(N, L), 0.0f);
		if (NdotL > 0.0f)
		{
			float D = DistributionGGX(N, H, Roughness);
			float NdotH = max(dot(N, H), 0.0f);
			float HdotV = max(dot(H, V), 0.0f);
			float pdf = ((D * NdotH) / (4.0f * HdotV)) + 0.0001f;

			float saTexel = 4.0f * PI / (6.0f * EnvironmentMapSize * EnvironmentMapSize);
			float saSample = 1.0f / (float(SAMPLE_COUNT) * pdf + 0.0001f);

			float mipLevel = Roughness == 0.0f ? 0.0f : 0.5f * log2(saSample / saTexel);
			prefilteredColor += EnvironmentMap.SampleLevel(LinearBorderSampler, L, mipLevel).rgb * NdotL;
			totalWeight += NdotL;
		}
	}

	prefilteredColor = prefilteredColor / totalWeight;
	return float4(prefilteredColor, 1.0f);
}
