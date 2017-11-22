#include "TextureLoader.h"
#include <png.h>

namespace Mile
{
   TextureInfo&& TextureLoader::LoadPNG( const String& filePath )
   {
      png_structp pngPtr = nullptr;
      png_infop infoPtr = nullptr;
      unsigned int sigRead = 8;
      int colorType, interlaceType;
      FILE* file = nullptr;

      file = _wfopen( filePath.c_str( ), TEXT( "rb" ) );
      if ( file == nullptr )
      {
         return TextureInfo( );
      }

      pngPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
                                       nullptr, nullptr, nullptr );
      if ( pngPtr == nullptr )
      {
         fclose( file );
         return TextureInfo( );
      }

      infoPtr = png_create_info_struct( pngPtr );
      if ( infoPtr == nullptr )
      {
         fclose( file );
         png_destroy_read_struct( &pngPtr, nullptr, nullptr );
         return TextureInfo( );
      }

      if ( setjmp( png_jmpbuf( pngPtr ) ) )
      {
         png_destroy_read_struct( &pngPtr, &infoPtr, nullptr );
         fclose( file );
         return TextureInfo( );
      }

      png_init_io( pngPtr, file );
      png_set_sig_bytes( pngPtr, sigRead );

      //png_read_png( pngPtr, infoPtr,
      //              PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND,
      //              nullptr );

      png_uint_32 width = png_get_image_width( pngPtr, infoPtr );
      png_uint_32 height = png_get_image_height( pngPtr, infoPtr );
      int bitDepth = png_get_bit_depth( pngPtr, infoPtr );

      png_uint_32 channels = png_get_channels( pngPtr, infoPtr );
      png_uint_32 colorType = png_get_color_type( pngPtr, infoPtr );
      //png_get_IHDR( pngPtr, infoPtr,
      //              &width, &height, &bitDepth, 
      //              &colorType, &interlaceType, nullptr, nullptr );

      switch ( colorType )
      {
      case PNG_COLOR_TYPE_PALETTE:
         png_set_palette_to_rgb( pngPtr );
         channels = 3;
         break;

      case PNG_COLOR_TYPE_GRAY:
         if ( bitDepth < 8 )
         {
            png_set_expand_gray_1_2_4_to_8( pngPtr );
         }

         bitDepth = 8;
         break;
      }

      if ( png_get_valid( pngPtr, infoPtr, PNG_INFO_tRNS ) )
      {
         png_set_tRNS_to_alpha( pngPtr );
         channels += 1;
      }

      if ( bitDepth == 16 )
      {
         png_set_strip_16( pngPtr );
      }

      png_read_update_info( pngPtr, infoPtr );

      unsigned int rowBytes = png_get_rowbytes( pngPtr, infoPtr );
      unsigned char* data = reinterpret_cast< unsigned char* >( malloc( rowBytes * height ) );
      png_bytepp rowPointers = png_get_rows( pngPtr, infoPtr );

      for ( size_t idx = 0; idx < height; ++idx )
      {
         memcpy( ( data + ( rowBytes * idx ) ), rowPointers[ idx ], rowBytes );
      }

      png_destroy_read_struct( &pngPtr, &infoPtr, nullptr );
      fclose( file );

      return std::make_tuple( data, width, height, channels );
   }
}