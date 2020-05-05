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

/* Constant Buffers (Pixel Shader) */
cbuffer SSAOParams : register(b0)
{
	float4 Samples[64];
	float2 NoiseScale;
};

cbuffer SSAOVariableParams : register(b1)
{
	matrix View;
	matrix Projection;
	float Radius;
	float Bias;
	float magnitude;
}

/* Textures & Samplers */
Texture2D posBuffer						: register(t0);
Texture2D albedoBuffer					: register(t1);
Texture2D emissiveAOBuffer				: register(t2);
Texture2D normalBuffer					: register(t3);
Texture2D metallicRoughnessBuffer	: register(t4);
Texture2D texNoise						: register(t5);
SamplerState Sampler						: register(s0);
SamplerState NoiseSampler				: register(s1);
SamplerState PositionSampler			: register(s2);

VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	output.PositionSS = input.Position;
	output.TexCoord = input.TexCoord;
	return output;
}

float MilePS(in PSInput input) : SV_Target0
{
	float3 pos = mul(View, float4(posBuffer.Sample(PositionSampler, input.TexCoord).xyz, 1.0f)).xyz;
	float3 normal = normalize(mul((float3x3)View, normalBuffer.Sample(Sampler, input.TexCoord).xyz));
	float3 randomVec = texNoise.Sample(NoiseSampler, input.TexCoord * NoiseScale).xyz;

	float3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	float3 bitangent = normalize(cross(normal, tangent));
	float3x3 tangentFrame = float3x3(tangent, bitangent, normal);

	const uint kernelSize = 64;
	float occlusion = 0.0f;
	for (uint idx = 0; idx < kernelSize; ++idx)
	{
		float3 _sample = mul(tangentFrame, Samples[idx].xyz);
		_sample = pos + (_sample * Radius);

		float4 offset = float4(_sample, 1.0f);
		offset = mul(Projection, offset);
		offset.xyz /= offset.w;
		offset.xy = offset.xy * 0.5f + 0.5f;
		offset.y = 1.0f-offset.y; // Direct3D Texture Coordinate

		float sampleDepth = mul(View, float4(posBuffer.Sample(PositionSampler, offset.xy).xyz, 1.0f)).z;
		float occluded = (_sample.z + Bias <= sampleDepth ? 0.0f : 1.0f);
		float intensity = smoothstep(0.0f, 1.0f, Radius / abs(pos.z - sampleDepth));
		occlusion +=  occluded * intensity;
	}

	occlusion = 1.0f - (occlusion / ((float)kernelSize));
	occlusion = pow(occlusion, magnitude);
	return float(occlusion);
}