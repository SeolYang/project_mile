#include "BRDFs.hlsli"

/* Input & Output Structures */
struct VSInput
{
	float4 Position		: POSITION;
	float2 TexCoord		: TEXCOORDS0;
};

struct VSOutput
{
	float4 PositionSS	: SV_Position;
	float2 TexCoord		: TEXCOORD;
};

struct PSInput
{
	float4 ScreenPos	: SV_Position;
	float2 TexCoord		: TEXCOORD;
};

/* Constant Buffers (Pixel Shader) */
cbuffer CameraParamsBuffer
{
	float3 CameraPos;
};

cbuffer AmbientParamsBuffer
{
	float Ao;
};

/* Textures & Samplers */
/* G-Buffer */
Texture2D posBuffer						: register(t0);
Texture2D albedoBuffer					: register(t1);
Texture2D emissiveAOBuffer				: register(t2);
Texture2D normalBuffer					: register(t3);
Texture2D metallicRoughnessBuffer	: register(t4);
/* IBL */
TextureCube irradianceMap				: register(t5);
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
	float3 worldPos = posBuffer.Sample(AnisoSampler, input.TexCoord).xyz;
	float3 albedo = albedoBuffer.Sample(AnisoSampler, input.TexCoord).rgb;
	float metallic = metallicRoughnessBuffer.Sample(AnisoSampler, input.TexCoord).b;

	float4 emissiveAO = emissiveAOBuffer.Sample(AnisoSampler, input.TexCoord).rgba;
	float3 emissive = emissiveAO.rgb;
	float ao = max(Ao, emissiveAO.a);

	float3 N = normalize(normalBuffer.Sample(AnisoSampler, input.TexCoord).xyz);
	float3 V = normalize(CameraPos - worldPos);

	float3 F0 = 0.04f;
	float3 kS = FresnelSchlick(max(dot(N, V), 0.0f), F0);
	float3 kD = 1.0f - kS;
	kD *= 1.0f - metallic;
	float3 irradiance = irradianceMap.Sample(AnisoSampler, N).rgb;
	float3 diffuse = irradiance * albedo;
	float3 ambient = (kD * diffuse) * ao;

	float3 color = ambient + emissive;
	return float4(color, 1.0);
}