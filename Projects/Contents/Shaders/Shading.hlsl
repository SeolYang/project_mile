// Textures
Texture2D      DiffuseMap     : register( t0 );
Texture2D      LightTexture   : register( t1 );
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
   float3 SpecularAlbedo;
};

cbuffer CheckerBoardBuffer
{
   bool CheckerBoardEnabled;
};

// Structures
struct VSInput
{
   float4 Position   : POSITION;
   float2 TexCoord   : TEXCOORDS0;
};

struct VSOutput
{
   float4 PositionCS : SV_Position;
   float2 TexCoord   : TEXCOORD;
};

struct PSInput
{
   float4 ScreenPos  : SV_Position;
   float2 TexCoord   : TEXCOORD;
};

VSOutput MileVS( in VSInput input )
{
   VSOutput output;
   output.PositionCS = mul( WorldViewProj, input.Position );
   output.TexCoord = input.TexCoord;

   return output;
}

float4 MilePS( in PSInput input ) : SV_Target0
{
   /* Checkerboard Rendering (Discard needless pixels) */
   bool discardPixel = ( ( input.ScreenPos.x + input.ScreenPos.y ) % 2 ) == 0;
   clip( CheckerBoardEnabled && discardPixel ? -1 : 1 );

   float3 diffuseAlbedo = DiffuseMap.Sample( AnisoSampler, input.TexCoord ).rgb;
   int3 sampleIndices = int3( input.ScreenPos.xy, 0 );

   float4 lighting = LightTexture.Load( sampleIndices );

   float3 diffuse = lighting.xyz * diffuseAlbedo;
   float3 specular = lighting.w * SpecularAlbedo;

   float3 ambient = diffuseAlbedo * float3( 0.05f, 0.05f, 0.05f );

   return float4( diffuse + specular + ambient, 1.0f );
}