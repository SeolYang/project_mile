// Light Pre Pass G-Buffer Shader
Texture2D      NormalMap      : register( t0 );
SamplerState   AnisoSampler   : register( s0 );

// Constants
cbuffer TransformBuffer
{
   matrix World;
   matrix WorldView;
   matrix WorldViewProj;
};

cbuffer MaterialPropertiesBuffer
{
   float SpecularPower;
};

// Structures
struct VSInput
{
   float4 Position      : POSITION;
   float2 TexCoord      : TEXCOORDS0;
   float3 Normal        : NORMAL;
   float4 Tangent       : TANGENT;
   float4 BiTangent     : BITANGENT;
};

// postfix WS meaning world space
struct VSOutput
{
   float4 PositionCS    : SV_Position; // Clip Space
   float2 TexCoord      : TEXCOORD;
   float3 NormalWS      : NORMALWS;
   float3 TangentWS     : TANGENTWS;
   float3 BitangentWS   : BITANGENTWS;
   float3 PositionWS    : POSITIONWS;
};

struct PSInput
{
   float4 PositionSS    : SV_Position; // Screen Space
   float2 TexCoord      : TEXCOORD;
   float3 NormalWS      : NORMALWS;
   float3 TangentWS     : TANGENTWS;
   float3 BitangentWS   : BITANGENTWS;
   float3 PositionWS    : POSITIONWS;
};

struct PSOutput
{
   float4 Normal        : SV_Target0;
   float4 Position      : SV_Target1;
};

SamplerState DefaultSampler
{
   AddressU = WRAP;
   AddressV = WRAP;
};

VSOutput MileVS( in VSInput input )
{
   VSOutput output;

   float3 normalWS = normalize( mul( ( float3x3 )World, input.Normal ) );
   float3 tangentWS = normalize( mul( ( float3x3 )World, input.Tangent.xyz ) );
   float3 bitangentWS = normalize( mul( ( float3x3 )World, input.BiTangent.xyz ) );

   output.NormalWS = normalWS;
   output.TangentWS = tangentWS;
   output.BitangentWS = bitangentWS;

   output.PositionWS = mul( World, input.Position ).xyz;
   output.PositionCS = mul( WorldViewProj, input.Position );
   output.TexCoord = input.TexCoord;

   return output;
}

PSOutput MilePS( in PSInput input )
{
   float3x3 tangentFrameWS = float3x3( normalize( input.TangentWS ),
                                       normalize( input.BitangentWS ),
                                       normalize( input.NormalWS ) );

   float3 normalTS = NormalMap.Sample( AnisoSampler, input.TexCoord ).rgb;
   normalTS = normalize( normalTS * 2.0f - 1.0f );

   float3 normalWS = mul( normalTS, tangentFrameWS );

   PSOutput output;
   output.Normal = float4( normalWS, SpecularPower );
   output.Position = float4( input.PositionWS, 1.0f );

   return output;
}