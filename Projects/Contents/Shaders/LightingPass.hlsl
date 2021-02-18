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

/* Constant Buffers */
cbuffer CameraParamsBuffer : register(b0)
{
	float3 CameraPos;
};

cbuffer LightParamsBuffer : register(b1)
{
	float4 LightPos;
	float4 LightDirection;
	float4 LightRadiance;
	uint	 LightType;
};

/* Textures & Samplers */
Texture2D posBuffer						: register(t0);
Texture2D albedoBuffer					: register(t1);
Texture2D emissiveAOBuffer				: register(t2);
Texture2D normalBuffer					: register(t3);
Texture2D extraComponents				: register(t4);
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
	float roughness = extraComponents.Sample(AnisoSampler, input.TexCoord).g;
	float metallic = extraComponents.Sample(AnisoSampler, input.TexCoord).b;

	float distance = length(LightPos.xyz - worldPos);
	float attenuation = 1.0f / (distance * distance);

	float3 N = normalize(normalBuffer.Sample(AnisoSampler, input.TexCoord).xyz);
	float3 V = normalize(CameraPos - worldPos);
	float3 L = normalize(LightPos.xyz - worldPos);
	if (LightType == 0) // Directional light
	{
		L = normalize(-LightDirection.xyz);
		attenuation = 1.0f;
	}

	float3 H = normalize(V + L);
	
	float3 F0 = 0.04f;
	F0 = lerp(F0, albedo, metallic);
	
	float3 Lo = 0.0f;
	float3 radiance = LightRadiance.rgb * attenuation;
	
	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);
	
	float3 nominator = NDF * G * F;
	float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
	float3 specular = nominator / max(denominator, 0.001f);
	
	float3 kS = F;
	float3 kD = 1.0f - kS;
	kD *= 1.0f - metallic;

	float NdotL = max(dot(N, L), 0.0f);
	Lo = (kD * albedo / PI + specular) * radiance * NdotL;
	return float4(Lo, 1.0f);
}