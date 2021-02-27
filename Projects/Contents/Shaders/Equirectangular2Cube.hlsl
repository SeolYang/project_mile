/* Shader In/output Structures **/
struct VSInput
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal	 : NORMAL;
};

struct VSOutput
{
	float4 Position	: SV_Position; // Clip Space
	float3 PositionWS : POSITIONWS; // Local Space
};

struct PSInput
{
	float4 Position	: SV_Position; // Screen Space
	float3 PositionWS : POSITIONWS;
};

/* Constant Buffers (Vertex Shader) **/
cbuffer TransfromBuffer
{
	float4x4 ViewProj;
};

/* Textures & Samplers */
Texture2D	 EquirectangularMap	: register(t0);
SamplerState LinearWrapSampler	: register(s0);

float2 SampleSphericalMap(float3 V)
{
	/** https://en.wikipedia.org/wiki/UV_mapping */
	const float2 InvAtan = float2(0.1591f, 0.3183f);
	float2 UV = float2(atan2(V.z, V.x), -asin(V.y));
	UV *= InvAtan;
	UV += float2(0.5f, 0.5f);
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
	float3 color = EquirectangularMap.Sample(LinearWrapSampler, uv).rgb;
	return float4(color, 1.0);
}
