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
            FIBITMAP* converted = FreeImage_ConvertTo32Bits(dib);
            FreeImage_FlipVertical(converted);
            auto width = FreeImage_GetWidth(converted);
            auto height = FreeImage_GetHeight(converted);
            auto channels = FreeImage_GetBPP(converted) / 8;
            auto data = FreeImage_GetBits(converted);

            FreeImage_Unload(dib);
            dib = nullptr;

            return { data, width, height, channels };
         }
      }

      return { };
   }
}