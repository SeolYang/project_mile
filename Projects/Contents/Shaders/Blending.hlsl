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
cbuffer BlendingParams : register(b0)
{
	float SrcRatio		: packoffset(c0);
	float DestRatio	: packoffset(c0.y);
};

/* Textures & Samplers */
Texture2D srcBuffer			: register(t0);
Texture2D destBuffer			: register(t1);
SamplerState Sampler			: register(s0);

VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionSS = input.Position;
	output.TexCoord = input.TexCoord;
	return output;
}

float4 MilePS(in PSInput input) : SV_Target0
{
	float3 srcColor = srcBuffer.Sample(Sampler, input.TexCoord).rgb * SrcRatio;
	float3 destColor = destBuffer.Sample(Sampler, input.TexCoord).rgb * DestRatio;

	return float4(srcColor + destColor, 1.0f);
}