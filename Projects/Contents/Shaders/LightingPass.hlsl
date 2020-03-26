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
	float4 LightPos;
	float4 LightDirection;
	float4 LightRadiance;
	uint LightType;
};

/* Textures & Samplers */
Texture2D posBuffer					: register(t0);
Texture2D albedoBuffer				: register(t1);
Texture2D emissiveAOBuffer			: register(t2);
Texture2D normalBuffer				: register(t3);
Texture2D metallicRoughnessBuffer	: register(t4);

SamplerState AnisoSampler			: register(s0);

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	
	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

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
	float4 emissiveAO = emissiveAOBuffer.Sample(AnisoSampler, input.TexCoord).rgba;
	float3 emissive = emissiveAO.rgb;
	float ao = emissiveAO.a;
	float roughness = metallicRoughnessBuffer.Sample(AnisoSampler, input.TexCoord).g;
	float metallic = metallicRoughnessBuffer.Sample(AnisoSampler, input.TexCoord).b;
	
	float3 N = normalize(normalBuffer.Sample(AnisoSampler, input.TexCoord).xyz);
	float3 V = normalize(CameraPos - worldPos);
	float3 L = normalize(LightPos.rgb - worldPos);
	float3 H = normalize(V + L);
	
	float3 F0 = 0.04;
	F0 = lerp(F0, albedo, metallic);
	
	float3 Lo = 0.0;
	float distance = length(LightPos.xyz - worldPos);
	float attenuation = 1.0 / (distance * distance);
	float3 radiance = LightRadiance * attenuation;
	
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);
	
	float3 nominator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
	float3 specular = nominator / denominator;
	
	float3 kS = F;
	float3 kD = 1.0- kS;
	kD *= 1.0 - metallic;
	
	float NdotL = max(dot(N, L), 0.0);
	Lo = (kD * albedo / PI + specular) * radiance * NdotL;
	
	float3 ambient = 0.03 * albedo * ao;
	float3 color = ambient + Lo + emissive;
	color = color / (color + 1.0);
	color = pow(color, (1.0 / 2.2));
	return float4(color, 1.0);
}