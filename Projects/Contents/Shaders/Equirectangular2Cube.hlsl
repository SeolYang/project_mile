/* Shader In/output Structures **/
struct VSInput
{
	float4 Position : POSITION;
};

struct VSOutput
{
	float4 PositionCS : SV_Position; // Clip Space
	float4 PositionLS : POSITIONSL; // Local Space
};

struct PSInput
{
	float4 PositionSS : SV_Position; // Screen Space
	float3 PositionLS : POSITIONLS;
};

/* Constant Buffers **/
cbuffer TransfromBuffer
{
	float4x4 View;
	float4x4 Proj;
};

/* Textures & Samplers */
Texture2D EquirectangularMap : register(t0);
SamplerState LinearClampSampler : register(s0);

float2 SampleSphericalMap(float3 V)
{
	const float2 InvAtan = float2(0.1591, 0.3183);
	float2 UV = float2(atan2(V.z, V.x), asin(V.y));
	UV *= InvAtan;
	UV += 0.5;
	return UV;
}

/* Shader Programs */
VSOutput MileVS(in VSInput input)
{
	VSOutput output;
	
	output.PositionCS = mul(Proj, mul(View, input.Position));
	output.PositionLS = input.Position;
	
	return output;
}

float4 MilePS(in PSInput input) : SV_Target0
{
	float2 uv = SampleSphericalMap(normalize(input.PositionLS));
	float3 color = EquirectangularMap.Sample(LinearClampSampler, uv).rgb;
	
	return float4(color, 1.0);
}
