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
	bool  Horizontal;
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
	const float HorizontalWeights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
	const float VerticalWeights[5] = { 0.304501f, 0.2945946f, 0.3216216f, 0.100023f, 0.022371f };
	uint width = 1;
	uint height = 1;
	uint mipLevels = 0;
	renderBuffer.GetDimensions(0, width, height, mipLevels);

	float2 texSize = float2(float(width), float(height));
	float2 texOffset = 1.0f / texSize;
	float3 color = renderBuffer.Sample(Sampler, input.TexCoord).rgb;
	float3 result = color * ((HorizontalWeights[0] + VerticalWeights[1]) / 2.0f);

	if (Horizontal)
	{
		for (int x = 1; x < 5; ++x)
		{
			result += renderBuffer.Sample(Sampler, input.TexCoord + float2(texOffset.x * x, 0.0f)).rgb * HorizontalWeights[x];
			result += renderBuffer.Sample(Sampler, input.TexCoord - float2(texOffset.x * x, 0.0f)).rgb * HorizontalWeights[x];
		}
	}
	else
	{
		for (int y = 1; y < 5; ++y)
		{
			result += renderBuffer.Sample(Sampler, input.TexCoord + float2(0.0f, texOffset.y * y)).rgb * VerticalWeights[y];
			result += renderBuffer.Sample(Sampler, input.TexCoord - float2(0.0f, texOffset.y * y)).rgb * VerticalWeights[y];
		}
	}

	return float4(result, 1.0f);
}