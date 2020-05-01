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
cbuffer ExtractPrams : register(b0)
{
	float3 Threshold;
};

/* Textures & Samplers */
Texture2D renderBuffer					: register(t0);
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
	float3 color = renderBuffer.Sample(Sampler, input.TexCoord);
	float brightness = dot(color, Threshold);
	if (brightness > 1.0f)
	{
		result = color;
	}

	return float4(result, 1.0f);
}