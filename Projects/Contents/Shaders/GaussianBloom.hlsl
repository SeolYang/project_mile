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
cbuffer BloomParams : register(b0)
{
	bool Horizontal;
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
	const float weights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
	uint width = 1;
	uint height = 1;
	uint mipLevels = 0;
	renderBuffer.GetDimensions(0, width, height, mipLevels);

	float2 texSize = float2(float(width), float(height));
	float2 texOffset = 1.0f / texSize;
	float3 result = renderBuffer.Sample(Sampler, input.TexCoord).rgb * weights[0];

	if (Horizontal)
	{
		for (int x = 1; x < 5; ++x)
		{
			result += renderBuffer.Sample(Sampler, input.TexCoord + float2(texOffset.x * x, 0.0f)).rgb * weights[x];
			result += renderBuffer.Sample(Sampler, input.TexCoord - float2(texOffset.x * x, 0.0f)).rgb * weights[x];
		}
	}
	else
	{
		for (int y = 1; y < 5; ++y)
		{
			result += renderBuffer.Sample(Sampler, input.TexCoord + float2(0.0f, texOffset.y * y)).rgb * weights[y];
			result += renderBuffer.Sample(Sampler, input.TexCoord - float2(0.0f, texOffset.y * y)).rgb * weights[y];
		}
	}

	return float4(result, 1.0f);
}