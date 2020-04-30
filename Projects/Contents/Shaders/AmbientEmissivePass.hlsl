#include "BRDFs.hlsli"

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
cbuffer AmbientParamsBuffer
{
	float3 CameraPos;
	float  Ao;
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
TextureCube prefilteredMap				: register(t6);
Texture2D brdfLUT							: register(t7);

SamplerState AnisoSampler				: register(s0);
SamplerState LinearClampSampler		: register(s1);

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
	float roughness = metallicRoughnessBuffer.Sample(AnisoSampler, input.TexCoord).g;
	float metallic = metallicRoughnessBuffer.Sample(AnisoSampler, input.TexCoord).b;

	float4 emissiveAO = emissiveAOBuffer.Sample(AnisoSampler, input.TexCoord).rgba;
	float3 emissive = emissiveAO.rgb;
	float ao = emissiveAO.a > 0.0f ? emissiveAO.a : Ao;

	float3 N = normalize(normalBuffer.Sample(AnisoSampler, input.TexCoord).xyz);
	float3 V = normalize(CameraPos - worldPos);
	float3 R = reflect(-V, N);

	float3 F0 = 0.04f;
	F0 = lerp(F0, albedo, metallic);
	float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, roughness);
	float3 kS = F;
	float3 kD = 1.0f - kS;
	kD *= 1.0f - metallic;

	float3 irradiance = irradianceMap.Sample(LinearClampSampler, N).rgb;
	float3 diffuse = irradiance * albedo;

	const float MAX_REFLECTION_LOD = 4.0f;
	float3 prefilteredColor = prefilteredMap.SampleLevel(LinearClampSampler, R, roughness * MAX_REFLECTION_LOD).rgb;
	float2 brdf = brdfLUT.Sample(LinearClampSampler, float2(max(dot(N, V), 0.0f), roughness)).rg;
	float3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	float3 ambient = (kD * diffuse + specular) * ao;
	float3 color = ambient + emissive;

	return float4(color, 1.0);
}