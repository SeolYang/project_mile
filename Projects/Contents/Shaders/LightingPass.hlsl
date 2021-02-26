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
	float3 LightPos : packoffset(c0);
	float	 LightIntensity : packoffset(c0.w);
	float3 LightDirection : packoffset(c1);
	float	 LightRadius : packoffset(c1.w);
	float3 LightColor : packoffset(c2);
	float  LightInnerAngle : packoffset(c2.w);
	float	 LightOuterAngle : packoffset(c3);
	uint	 LightType : packoffset(c3.y);
};

/* Textures & Samplers */
Texture2D posBuffer						: register(t0);
Texture2D albedoBuffer					: register(t1);
Texture2D emissiveBuffer				: register(t2);
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

float SquareFalloffAttenuation(float distance, float lightRadius)
{
	float distanceSquare = (distance * distance);
	float lightRadiusInv = 1.0f / lightRadius;
	float factor = distanceSquare * lightRadiusInv * lightRadiusInv;
	float smoothFactor = saturate(1.0f - (factor * factor));

	return (smoothFactor * smoothFactor) / max(distanceSquare, 1e-4);
}

float SpotAngleAttenuation(float3 surfaceToLight, float3 lightDir, float innerAngle, float outerAngle)
{
	// Inner 안에 있을때는 최대 밝기
	float cosOuterAngle = cos(outerAngle);
	float cosInnerAngle = cos(innerAngle);
	float spotScale = 1.0f / max(cosInnerAngle - cosOuterAngle, 1e-4);
	float spotOffset = -cosOuterAngle * spotScale;

	float dotDirs = dot(normalize(surfaceToLight), normalize(lightDir));
	float attenuation = saturate(dotDirs * spotScale + spotOffset);
	return (attenuation * attenuation);
}

float4 MilePS(in PSInput input) : SV_Target0
{
	float3 worldPos = posBuffer.Sample(AnisoSampler, input.TexCoord).xyz;
	float3 albedo = albedoBuffer.Sample(AnisoSampler, input.TexCoord).rgb;
	float roughness = extraComponents.Sample(AnisoSampler, input.TexCoord).g;
	float metallic = extraComponents.Sample(AnisoSampler, input.TexCoord).b;
	float specularFactor = 0.08 * extraComponents.Sample(AnisoSampler, input.TexCoord).a;

	float distance = length(LightPos.rgb - worldPos);
	float attenuation = SquareFalloffAttenuation(distance, LightRadius);

	float3 N = normalize(normalBuffer.Sample(AnisoSampler, input.TexCoord).xyz);
	float3 V = normalize(CameraPos - worldPos);
	float3 L = normalize(LightPos.xyz - worldPos);
	if (LightType == 0) // Directional light
	{
		L = normalize(-LightDirection.xyz);
		attenuation = 1.0f;
	}
	else if (LightType == 2) // Spot Light
	{
		attenuation *= SpotAngleAttenuation(L, -LightDirection.xyz, LightInnerAngle, LightOuterAngle);
	}

	float3 H = normalize(V + L);
	
	float3 F0 = specularFactor.xxx;
	F0 = lerp(F0, albedo, metallic);
	
	float3 Lo = 0.0f;
	float NdotL = max(dot(N, L), 0.0f);
	float3 radiance = LightColor.rgb * attenuation * (NdotL * LightIntensity);
	
	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);
	
	float3 nominator = NDF * G * F;
	float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
	float3 specular = nominator / max(denominator, 0.001f);

	// Lambert Diffuse BRDF
	float3 kS = F;
	float3 kD = 1.0f - kS;
	kD *= 1.0f - metallic;
	float3 diffuse = (kD * albedo) / PI;

	Lo = (diffuse + specular) * radiance;
	return float4(Lo, 1.0f);
}