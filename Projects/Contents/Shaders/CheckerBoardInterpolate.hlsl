// Textures
Texture2D   CheckerBoardTexture   : register( t0 );

cbuffer RenderTargetInfoBuffer
{
   int Width;
   int Height;
};

float4 MileVS( in float4 Position : POSITION ) : SV_Position
{
   return Position;
}

float4 MilePS(in float4 ScreenPos : SV_Position) : SV_TARGET
{
   float3 result = float3( 0.0f, 0.0f, 0.0f );

   bool discardedPixel = ( ( ScreenPos.x + ScreenPos.y ) % 2 ) == 0;
   if ( discardedPixel )
   {
      float dx1 = ScreenPos.x - 1.0f;
      float dx2 = ScreenPos.x + 1.0f;
      float dy1 = ScreenPos.y - 1.0f;
      float dy2 = ScreenPos.y + 1.0f;

      int3 sampleLeft = int3( dx1, ScreenPos.y, 0 );
      int3 sampleRight = int3( dx2, ScreenPos.y, 0 );
      int3 sampleUp = int3( ScreenPos.x, dy1, 0 );
      int3 sampleDown = int3( ScreenPos.x, dy2, 0 );

      float sampleCount = 0.0f;

      if ( dx1 > 0.0f )
      {
         result += CheckerBoardTexture.Load( sampleLeft ).rgb;
         sampleCount += 1.0f;
      }
      if ( dx2 < Width )
      {
         result += CheckerBoardTexture.Load( sampleRight ).rgb;
         sampleCount += 1.0f;
      }
      if ( dy1 > 0 )
      {
         result += CheckerBoardTexture.Load( sampleUp ).rgb;
         sampleCount += 1.0f;
      }
      if ( dy2 < Height )
      {
         result += CheckerBoardTexture.Load( sampleDown ).rgb;
         sampleCount += 1.0f;
      }

      result /= sampleCount;
   }
   else
   {
      result = CheckerBoardTexture.Load( int3( ScreenPos.x, ScreenPos.y, 0 ) ).rgb;
   }

   return float4( result, 1.0f );
}