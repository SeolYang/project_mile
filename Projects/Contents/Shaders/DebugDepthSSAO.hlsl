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

cbuffer TypeBuffer
{
	uint Type;
	float2 CameraParams;
};

/* Textures & Samplers */
Texture2D srcBuffer			: register(t0);
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
	float srcColor = srcBuffer.Sample(Sampler, input.TexCoord).r;
	if (Type == 0)
	{
		srcColor = LinearizeDepth(srcColor, CameraParams.x, CameraParams.y);
	}
	return float4(srcColor, srcColor, srcColor, 1.0f);
}