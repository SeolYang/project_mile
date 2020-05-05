#include "Common.hlsli"

/* Input & Output Structures */
struct VSInput
{
	float4 Position		: POSITION;
	float2 TexCoord		: TEXCOORDS0;
};

struct VSOutput
{
	float4 PositionSS		: SV_Position;
	float2 TexCoord		: TEXCOORD;
};

struct PSInput
{
	float4 ScreenPos		: SV_Position;
	float2 TexCoord		: TEXCOORD;
};

/* Constant Buffers (Pixel Shader) */
cbuffer ExtractParams : register(b0)
{
	float Threshold;
};

/* Textures & Samplers */
Texture2D posBuffer						: register(t0);
Texture2D albedoBuffer					: register(t1);
Texture2D emissiveAOBuffer				: register(t2);
Texture2D normalBuffer					: register(t3);
Texture2D metallicRoughnessBuffer	: register(t4);
Texture2D renderBuffer					: register(t5);
SamplerState Sampler						: register(s0);

VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionSS = input.Position;
	output.TexCoord = input.TexCoord;
	return output;
}

float4 MilePS(in PSInput input) : SV_Target0
{
	float3 result = 0.0f;
	float3 color = renderBuffer.Sample(Sampler, input.TexCoord).rgb;
	float brightness = max(color.r, max(color.g, color.b));
	if (brightness > Threshold)
	{
		result = color;
	}

	return float4(result, 1.0f);
}