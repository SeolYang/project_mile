Texture2D      DiffuseMap     : register( t0 );
SamplerState   AnisoSampler   : register( s0 );

// Constants
cbuffer TransformBuffer
{
   matrix World;
   matrix WorldView;
   matrix WorldViewProj;
};

struct VSInput
{
   float4 Position      : POSTIION;
   float2 TexCoord      : TEXCOORDS0;
};

struct VSOutput
{
   float4 PositionCS : SV_Position;
   float2 TexCoord :TEXCOORD;
};

struct PSInput
{
   float4 PostionSS : SV_Position;
   float2 TexCoord : TEXCOORD;
};

VSOutput MileVS( in VSInput input )
{
   VSOutput output;
   //output.PositionCS = mul( input.Position, WorldViewProj );
   output.PositionCS = mul( WorldViewProj, input.Position );
   output.TexCoord = input.TexCoord;
   return output;
}

float4 MilePS( in PSInput input ) : SV_Target0
{
   float3 diffuseAlbedo = DiffuseMap.Sample( AnisoSampler, input.TexCoord ).rgb;
   return float4( diffuseAlbedo, 1.0f );
}