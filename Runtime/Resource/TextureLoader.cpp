#include "Resource/TextureLoader.h"
#include <fstream>
#include <iostream>
#include <FreeImage.h>

namespace Mile
{
   TextureInfo TextureLoader::LoadTexture(const String& inFilePath, const String& fileExtension)
   {
      std::string filePath = WString2String(inFilePath);
      FreeImage_Initialise(TRUE);

      auto fif = FreeImage_GetFIFFromFilename(filePath.c_str());
      if (fif != FIF_UNKNOWN)
      {
         FIBITMAP* dib = FreeImage_Load(fif, filePath.c_str());

         if (dib != nullptr)
         {
            FIBITMAP* converted = dib;
            bool bIsHDRTexture = fileExtension == TEXT("hdr");
            unsigned int bitPerChannel = bIsHDRTexture ? 32 : 8;
            if (!bIsHDRTexture)
            {
               converted = FreeImage_ConvertTo32Bits(dib);
            }
            else
            {
               converted = FreeImage_ConvertToRGBAF(dib);
            }
            FreeImage_FlipVertical(converted);
            auto width = FreeImage_GetWidth(converted);
            auto height = FreeImage_GetHeight(converted);
            auto channels = FreeImage_GetBPP(converted) / bitPerChannel;
            auto data = FreeImage_GetBits(converted);

            FreeImage_Unload(dib);
            dib = nullptr;

            return { data, width, height, channels, bitPerChannel, bIsHDRTexture };
         }
      }

      return { };
   }
}