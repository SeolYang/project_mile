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

/* Constant Buffers */
cbuffer CameraParamsBuffer
{
	float3 CameraPos;
};

cbuffer LightParamsBuffer
{
	float3 LightPos;
	float3 LightRadiance;
	float3 LightDirection;
	uint LightType;
};

/* Textures & Samplers */
Texture2D posBuffer					: register(t0);
Texture2D albedoBuffer				: register(t1);
Texture2D emissiveAOBuffer			: register(t2);
Texture2D normalBuffer				: register(t3);
Texture2D metallicRoughnessBuffer	: register(t4);

SamplerState AnisoSampler			: register(s0);

/* Functions */
float SmithGGX(float3 normal, float3 view, float3 light, float roughness)
{
	float remappingFactor = 8.0;
	float ggx1 = SchlickG1(normal, view, roughness, remappingFactor);
	float ggx2 = SchlickG1(normal, light, roughness, remappingFactor);

	return (ggx1 * ggx2);
}

/* Shader Programs */
VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionSS = input.Position;
	output.TexCoord = input.TexCoord;
	return output;
}

float4 MilePS(in PSInput input) : SV_Target0
{
	float3 position = posBuffer.Sample(AnisoSampler, input.TexCoord).xyz;
	
	float3 albedo = albedoBuffer.Sample(AnisoSampler, input.TexCoord).rgb;
	
	float4 emissiveAO = emissiveAOBuffer.Sample(AnisoSampler, input.TexCoord).rgba;
	float ao = emissiveAO.a;
	
	float3 emissive = emissiveAO.rgb;
	float3 ambient = 0.03 * albedo * ao;
	
	float3 normal = normalize(normalBuffer.Sample(AnisoSampler, input.TexCoord).xyz);
	
	float roughness = metallicRoughnessBuffer.Sample(AnisoSampler, input.TexCoord).g;
	float metallic = metallicRoughnessBuffer.Sample(AnisoSampler, input.TexCoord).b;

	float3 Lo = 0.0;
	float3 view = normalize(CameraPos - position);
	float3 light = normalize(LightPos - position);
	
	if (LightType == 0)
	{
		light = normalize(LightDirection);
	}
	
	float3 halfway = normalize(view + light);
	
	float lightDistance = length(LightPos - position);
	float attenuation = 1.0 / (lightDistance * lightDistance);
	
	float3 radiance = LightRadiance * attenuation;
	
	/* Fresnel Term */
	float3 F0 = 0.04;
	F0 = lerp(F0, albedo, metallic);
	float3 F = FresnelSchlick(max(dot(halfway, view), 0.0), F0);

	/* NDF Term */
	float NDF = TrowbridgeReitzNDF(normal, halfway, roughness);
	
	/* GS Term */
	float G = SmithGGX(normal, view, light, roughness);
	
	/* BRDF */
	float3 nominator = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, light), 0.0);
	
	float specular = nominator / max(denominator, 0.0001);
	
	float3 kS = F;
	float3 kD = float3(1.0, 1.0, 1.0) - kS;
	kD *= 1.0 - metallic;
	
	Lo = ((kD * albedo / PI) + specular) * radiance * dot(normal, light);
	
	float3 color = ambient + emissive + Lo;
	color = color / (color + float3(1.0f));
	color = pow(color, float3(1.0 / 2.0));

	/* Lighting Pass 한번 당 한개의 라이트를 처리하므로, Additive Blend 를 해주어야 최종 결과물에 라이팅 결과가 올바르게 반영됨. */
	return float4(color, 1.0);
}