#pragma once

#include <tuple>
#include "Core\Helper.h"

namespace Mile
{
   // <RawData, Width, Height, Channels>
   using TextureInfo = std::tuple<unsigned char*, unsigned int, unsigned int, int>;
   enum TextureInfoTag
   {
      TIT_RAWDATA = 0,
      TIT_WIDTH = 1,
      TIT_HEIGHT = 2,
      TIT_CHANNELS = 3,
   };

   class MEAPI TextureLoader
   {
   public:
      static TextureInfo LoadTexture( const String& filePath, const String& fileExtension )
      {
         if ( fileExtension == TEXT( "png" ) )
         {
            return std::move( LoadPNG( filePath ) );
         }

         return TextureInfo( );
      }

      static TextureInfo LoadPNG( const String& filePath );

   };
}