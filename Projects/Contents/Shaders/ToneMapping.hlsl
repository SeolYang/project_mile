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
	float ToneMappingFactor;
	float GammaFactor;
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
	float3 color = renderBuffer.Sample(AnisoSampler, input.TexCoord);
	color = color / (color + ToneMappingFactor); /* Tone mapping **/
	color = pow(color, (1.0f / GammaFactor)); /* Gamma-Correction **/
	return float4(color, 1.0f);
}