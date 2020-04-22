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

/* Constant Buffers **/
cbuffer TransfromBuffer
{
	float4x4 ViewProj;
};

/* Textures & Samplers */
Texture2D EquirectangularMap : register(t0);
SamplerState LinearClampSampler : register(s0);

float2 SampleSphericalMap(float3 V)
{
	/** https://en.wikipedia.org/wiki/UV_mapping */
	const float2 InvAtan = float2(0.1591, 0.3183);
	float2 UV = float2(atan2(V.z, V.x), asin(V.y));
	UV *= InvAtan;
	UV += float2(0.5, 0.5);
	return UV;
}

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
	float2 uv = SampleSphericalMap(normalize(input.PositionWS).xyz);
	float3 color = EquirectangularMap.Sample(LinearClampSampler, uv).rgb;
	return float4(color, 1.0);
}
