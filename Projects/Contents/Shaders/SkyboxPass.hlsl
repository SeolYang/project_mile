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
	float4x4 View;
	float4x4 Proj;
};

TextureCube EnvironmentMap;
SamplerState EnvironmentSampler;

VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionLS = input.Position.xyz;

	float4x4 rotView = remove_translation(View);
	float4 clipPosition = mul(Proj, mul(rotView, input.Position));
	output.PositionCS = clipPosition.xyww;
	return output;
}

float4 MilePS(in PSInput input) : SV_Target0
{
	float3 color = EnvironmentMap.Sample(EnvironmentSampler, input.PositionLS).rgb;
	color = color / (color + float3(1.0f, 1.0f, 1.0f));
	color = pow(color, (1.0f / 2.2f));
	return float4(color, 1.0f);
}