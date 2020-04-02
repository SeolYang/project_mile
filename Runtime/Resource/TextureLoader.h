#pragma once
#include "Core\Helper.h"
#include <tuple>

namespace Mile
{
   /**
    * @brief	텍스쳐에 대한 정보를 담고있는 튜플 객체 입니다. <RawData, Width, Height, Channels, bit per channel, is hdr>
    */
   using TextureInfo = std::tuple<unsigned char*, unsigned int, unsigned int, int, unsigned int, bool>;
   enum TextureInfoTag
   {
      RAWDATA = 0,
      WIDTH = 1,
      HEIGHT = 2,
      CHANNELS = 3,
      BIT_PER_CHANNEL,
      IS_HDR_TEXTURE
   };

   class MEAPI TextureLoader
   {
   public:
      static TextureInfo LoadTexture(const String& inFilePath, const String& fileExtension);
   };
}