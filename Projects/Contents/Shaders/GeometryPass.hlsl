/* Geometry Pass */
#include "Common.hlsli"

struct VSInput
{
	float4 Position		: POSITION;
	float2 TexCoord		: TEXCOORDS0;
	float3 Normal		: NORMAL;
	float4 Tangent		: TANGENT;
	float4 BiTangent	: BITANGENT;
};

// postfix WS meaning world space
struct VSOutput
{
	float4 PositionCS	: SV_Position; // Clip Space
	float2 TexCoord		: TEXCOORD;
	float3 NormalWS		: NORMALWS;
	float3 TangentWS	: TANGENTWS;
	float3 BitangentWS	: BITANGENTWS;
	float3 PositionWS	: POSITIONWS;
};

struct PSInput
{
	float4 PositionSS	: SV_Position; // Screen Space
	float2 TexCoord		: TEXCOORD;
	float3 NormalWS		: NORMALWS;
	float3 TangentWS	: TANGENTWS;
	float3 BitangentWS	: BITANGENTWS;
	float3 PositionWS	: POSITIONWS;
};

struct PSOutput
{
	float4 Position				: SV_Target0;
	float4 Albedo				: SV_Target1;
	float4 EmissiveAO			: SV_Target2;
	float4 Normal				: SV_Target3;
	float4 MetallicRoughness	: SV_Target4;
};

/* Textures & Samplers */
Texture2D baseColorMap			: register(t0);
Texture2D emissiveMap			: register(t1);
Texture2D metallicRoughnessMap	: register(t2);
Texture2D aoMap					: register(t3);
Texture2D normalMap				: register(t4);

SamplerState AnisoSampler		: register(s0);

/* Constant Buffers */
cbuffer TransformBuffer
{
	matrix World;
	matrix WorldView;
	matrix WorldViewProj;
};

cbuffer MaterialBuffer
{
	float4	baseColorFactor;
	float4	emissiveFactor;
	float	metallicFactor;
	float	roughnessFactor;
}

/* Shader Programs */
VSOutput MileVS(in VSInput input)
{
	VSOutput output;

	float3 normalWS = normalize(mul((float3x3) World, input.Normal));
	float3 tangentWS = normalize(mul((float3x3) World, input.Tangent.xyz));
	float3 bitangentWS = normalize(mul((float3x3) World, input.BiTangent.xyz));

	output.NormalWS = normalWS;
	output.TangentWS = tangentWS;
	output.BitangentWS = bitangentWS;

	output.PositionWS = mul(World, input.Position).xyz;
	output.PositionCS = mul(WorldViewProj, input.Position);
	output.TexCoord = input.TexCoord;

	return output;
}

PSOutput MilePS(in PSInput input)
{
	/* Albedo */
	float3 albedo = pow(baseColorMap.Sample(AnisoSampler, input.TexCoord), 2.2);
	albedo += pow(baseColorFactor, 2.2);
	
	/* Emissive */
	float3 emissive = pow(emissiveMap.Sample(AnisoSampler, input.TexCoord), 2.2);
	emissive += pow(emissiveFactor, 2.2);
	
	/* Metallic-Roughness-UVs */
	float roughness = metallicRoughnessMap.Sample(AnisoSampler, input.TexCoord).g;
	roughness = clamp(roughnessFactor + roughness, 0.0f, 1.0f);
	
	float metallic = metallicRoughnessMap.Sample(AnisoSampler, input.TexCoord).b;
	metallic = clamp(metallicFactor + metallic, 0.0, 1.0f);
	
	/* AO */
	float ao = aoMap.Sample(AnisoSampler, input.TexCoord).r;
	
	/* Normal */
	float3 normalWS = normalize(input.NormalWS);
	float3x3 tangentFrameWS = float3x3(normalize(input.TangentWS),
                                       normalize(input.BitangentWS),
                                       normalWS);
	

	float3 normalTS = normalMap.Sample(AnisoSampler, input.TexCoord).rgb;
	normalTS = normalize(normalTS * 2.0f - 1.0f);
	float3 normal = any(normalTS) ? normalWS : mul(normalTS, tangentFrameWS);

	PSOutput output;
	output.Position = float4(input.PositionWS, 1.0f);
	output.Albedo = float4(albedo, 1.0f);
	output.EmissiveAO = float4(emissive, ao);
	output.Normal = float4(normal, 1.0f);
	output.MetallicRoughness = float4(0.0f, roughness, metallic, 1.0f);

	return output;
}
