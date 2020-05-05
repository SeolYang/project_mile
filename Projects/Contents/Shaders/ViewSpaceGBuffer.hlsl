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

struct PSOutput
{
	float4 Position				: SV_Target0;
	float4 Albedo					: SV_Target1;
	float4 EmissiveAO				: SV_Target2;
	float4 Normal					: SV_Target3;
	float4 MetallicRoughness	: SV_Target4;
};

/* Constant Buffers (Pixel Shader) */
cbuffer ConvertParams : register(b0)
{
	matrix View;
};

/* Textures & Samplers */
Texture2D posBuffer						: register(t0);
Texture2D albedoBuffer					: register(t1);
Texture2D emissiveAOBuffer				: register(t2);
Texture2D normalBuffer					: register(t3);
Texture2D metallicRoughnessBuffer	: register(t4);
Texture2D depthBuffer					: register(t5);
SamplerState Sampler						: register(s0);

VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionSS = input.Position;
	output.TexCoord = input.TexCoord;
	return output;
}

PSOutput MilePS(in PSInput input)
{
	PSOutput output;

	float3 position = posBuffer.Sample(Sampler, input.TexCoord).xyz;
	output.Position = float4(mul(View, float4(position, 1.0f)).xyz, 1.0f);
	output.Albedo = albedoBuffer.Sample(Sampler, input.TexCoord).rgba;
	output.EmissiveAO = emissiveAOBuffer.Sample(Sampler, input.TexCoord).rgba;
	float3 normal = normalBuffer.Sample(Sampler, input.TexCoord).xyz;
	output.Normal = float4(mul((float3x3)View, normal.xyz).xyz, 0.0f);
	output.MetallicRoughness = float4(metallicRoughnessBuffer.Sample(Sampler, input.TexCoord).rg, 0.0f, 0.0f);
	return output;
}