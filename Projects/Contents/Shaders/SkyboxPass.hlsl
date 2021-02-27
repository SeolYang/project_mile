#include "Common.hlsli"

struct VSInput
{
	float4 Position : POSITION;
};

struct VSOutput
{
	float4 PositionCS : SV_Position;
	float3 PositionLS : POSITIONLS;
};

struct PSInput
{
	float4 PositionSS : SV_Position;
	float3 PositionLS : POSITIONLS;
};

/* Vertex Shader Constant Buffer **/
cbuffer TransformBuffer
{
	float4x4 ViewProj;
};

/* Pixel Shader CB **/
cbuffer IntensityBuffer
{
	float PreExposedIBLIntensity;
};

TextureCube EnvironmentMap;
SamplerState EnvironmentSampler;

VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionLS = input.Position.xyz;

	float4 clipPosition = mul(ViewProj, input.Position);
	output.PositionCS = clipPosition.xyww;
	return output;
}

float4 MilePS(in PSInput input) : SV_Target0
{
	float3 color = EnvironmentMap.Sample(EnvironmentSampler, input.PositionLS).rgb * PreExposedIBLIntensity;
	return float4(color, 1.0f);
}