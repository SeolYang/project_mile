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

/* Textures & Samplers */
Texture2D ssaoInput						: register(t0);
SamplerState Sampler						: register(s0);

VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionSS = input.Position;
	output.TexCoord = input.TexCoord;
	return output;
}

float MilePS(in PSInput input) : SV_Target0
{
	uint width = 1;
	uint height = 1;
	uint mipLevels = 0;
	ssaoInput.GetDimensions(0, width, height, mipLevels);
	float2 texelSize = 1.0f / float2(width, height);

	float result = 0.0f;
	for (int x = -2; x < 2; ++x)
	{
		for (int y = -2; y < 2; ++y)
		{
			float2 offset = float2(float(x), float(y)) * texelSize;
			result += ssaoInput.Sample(Sampler, input.TexCoord + offset).r;
		}
	}

	return (result / (3.0f * 3.0f));
}