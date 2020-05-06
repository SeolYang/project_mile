/* Geometry Pass */
#include "Common.hlsli"

struct VSInput
{
	float4 Position		: POSITION;
	float2 TexCoord		: TEXCOORDS0;
	float3 Normal			: NORMAL;
	float4 Tangent			: TANGENT;
	float4 BiTangent		: BITANGENT;
};

// postfix WS meaning world space
struct VSOutput
{
	float4 PositionCS		: SV_Position; // Clip Space
	float2 TexCoord		: TEXCOORD;
	float3 NormalWS		: NORMALWS;
	float3 TangentWS		: TANGENTWS;
	float3 BitangentWS	: BITANGENTWS;
	float3 PositionWS		: POSITIONWS;
};

struct PSInput
{
	float4 PositionSS		: SV_Position; // Screen Space
	float2 TexCoord		: TEXCOORD;
	float3 NormalWS		: NORMALWS;
	float3 TangentWS		: TANGENTWS;
	float3 BitangentWS	: BITANGENTWS;
	float3 PositionWS		: POSITIONWS;
};

struct PSOutput
{
	float4 Position				: SV_Target0;
	float4 Albedo					: SV_Target1;
	float4 EmissiveAO				: SV_Target2;
	float4 Normal					: SV_Target3;
	/* R : Reflectivity, G : ROUGHNESS, B : METALLIC, A : Specular */
	float4 ExtraComponents		: SV_Target4;
};

/* Textures & Samplers */
Texture2D baseColorMap				: register(t0);
Texture2D emissiveMap				: register(t1);
Texture2D metallicRoughnessMap	: register(t2);
Texture2D specularMap				: register(t3);
Texture2D aoMap						: register(t4);
Texture2D normalMap					: register(t5);
SamplerState Sampler					: register(s0);

/* Constant Buffers (Vertex Shader) */
cbuffer TransformBuffer
{
	matrix World;
	matrix WorldView;
	matrix WorldViewProj;
};

/* Constant Buffers (Pixel Shader) */
cbuffer MaterialBuffer
{
	float4	baseColorFactor	: packoffset(c0);
	float4	emissiveFactor		: packoffset(c1);
	float		metallicFactor		: packoffset(c2);
	float		roughnessFactor	: packoffset(c2.y);
	float2	uvOffset				: packoffset(c2.z);
	float		specularFactor		: packoffset(c3);
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
	float2 uv = input.TexCoord + uvOffset;
	/* Albedo */
	float3 albedo = pow(baseColorMap.Sample(Sampler, uv), 2.2).rgb;
	albedo += pow(baseColorFactor, 2.2);
	
	/* Emissive */
	float3 emissive = pow(emissiveMap.Sample(Sampler, uv), 2.2).rgb;
	emissive += pow(emissiveFactor, 2.2);
	
	/* Metallic-Roughness */
	float roughness = metallicRoughnessMap.Sample(Sampler, uv).g;
	roughness = clamp(roughnessFactor + roughness, 0.0f, 1.0f);
	
	float metallic = metallicRoughnessMap.Sample(Sampler, uv).b;
	metallic = clamp(metallicFactor + metallic, 0.0f, 1.0f);

	/* Specular */
	float specular = specularMap.Sample(Sampler, uv).r;
	specular = clamp(specularFactor + specular, 0.0f, 1.0f);

	/* Reflectivity */
	const float reflectivityThreshold = 0.01f;
	float reflectivity = metallic > reflectivityThreshold ? metallic : specular;
	
	/* AO */
	float ao = aoMap.Sample(Sampler, uv).r;
	
	/* Normal */
	float3 normalWS = normalize(input.NormalWS);
	float3x3 tangentFrameWS = float3x3(normalize(input.TangentWS),
                                       normalize(input.BitangentWS),
                                       normalWS);
	

	float3 normalTS = normalMap.Sample(Sampler, uv).rgb;
	normalTS = normalize(normalTS * 2.0f - 1.0f);
	float3 normal = all(normalTS) ? normalWS : mul(normalTS, tangentFrameWS);

	PSOutput output;
	output.Position = float4(input.PositionWS, 1.0f);
	output.Albedo = float4(albedo, 1.0f);
	output.EmissiveAO = float4(emissive, ao);
	output.Normal = float4(normal, 1.0f);
	output.ExtraComponents = float4(reflectivity, roughness, metallic, specular);

	return output;
}
