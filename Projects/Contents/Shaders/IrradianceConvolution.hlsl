#include "Common.hlsli"

/* Shader In/output Structures **/
struct VSInput
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct VSOutput
{
	float4 Position : SV_Position; // Clip Space
	float3 PositionWS : POSITIONWS; // Local Space
};

struct PSInput
{
	float4 Position : SV_Position; // Screen Space
	float3 PositionWS : POSITIONWS;
};

/* Constant Buffers (Vertex Shader) **/
cbuffer TransfromBuffer : register(b0)
{
	float4x4 ViewProj;
};

/* Textures & Samplers */
TextureCube EnvironmentMap : register(t0);
SamplerState LinearWrapSampler : register(s0);

/* Shader Programs */
VSOutput MileVS(in VSInput input)
{
	VSOutput output;

	output.Position = mul(ViewProj, input.Position);
	output.PositionWS = input.Position.xyz;

	return output;
}

float4 MilePS(in PSInput input) : SV_Target0
{
	float3 normal = normalize(input.PositionWS);
	float3 irradiance = float3(0.0f, 0.0f, 0.0f);

	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(up, normal);
	up = cross(right, normal);

	float sampleDelta = 0.025f;
	float nrSamples = 0.0f;
	for (float phi = 0.0f; phi < (2.0f * PI); phi += sampleDelta)
	{
		for (float theta = 0.0f; theta < (0.5f * PI); theta += sampleDelta)
		{
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);
			float3 tangentSample = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += EnvironmentMap.Sample(LinearWrapSampler, sampleVec).rgb * cosTheta * sinTheta;
			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0f / float(nrSamples));
	return float4(irradiance, 1.0f);
}
