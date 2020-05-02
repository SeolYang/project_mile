/* Tone Mapping with gamma correction **/
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
cbuffer ToneMappingParams : register(b0)
{
	float ExposureFactor	: packoffset(c0);
	float GammaFactor		: packoffset(c0.y);
};

/* Textures & Samplers */
Texture2D renderBuffer					: register(t0);
SamplerState AnisoSampler				: register(s0);

VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionSS = input.Position;
	output.TexCoord = input.TexCoord;
	return output;
}

float4 MilePS(in PSInput input) : SV_Target0
{
	float3 color = renderBuffer.Sample(AnisoSampler, input.TexCoord).rgb;
	float3 mappedColor;
	/** Exposure Tone Mapping */
	if (ExposureFactor >= 0.0f)
	{
		mappedColor = float3(1.0f, 1.0f, 1.0f) - exp(-color * ExposureFactor);
	}

	/** Gamma Correction */
	if (GammaFactor >= 0.0f)
	{
		mappedColor = pow(mappedColor, (1.0f / GammaFactor));
	}

	return float4(mappedColor, 1.0f);
}