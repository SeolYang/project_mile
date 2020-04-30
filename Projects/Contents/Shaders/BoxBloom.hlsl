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
	int Size;
	float Separation;
	float Threshold;
	float Amount;
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
	if (Size <= 0)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	uint width = 1;
	uint height = 1;
	uint mipLevels = 0;
	renderBuffer.GetDimensions(0, width, height, mipLevels);

	float2 texSize = float2(float(width), float(height));
	float value = 0.0f;
	float4 result = 0.0f;
	float4 color = 0.0f;

	for (int x = -Size; x <= Size; ++x)
	{
		for (int y = -Size; y <= Size; ++y)
		{
			float2 uv = input.TexCoord + (float2(x, y) * Separation) / texSize;
			color = renderBuffer.Sample(Sampler, uv);

			value = max(color.r, max(color.g, color.b));
			if (value < Threshold)
			{
				color = 0.0f;
			}

			result.rgb += color.rgb;
		}
	}

	result.rgb /= pow(Size * 2.0f + 1.0f, 2.0f);
	result.a = 1.0f;

	return float4(lerp(float3(0.0f ,0.0f, 0.0f), result.rgb, Amount), 1.0f);
}