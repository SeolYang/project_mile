#pragma once
#include "Rendering/CubemapBase.h"

namespace Mile
{
   class MEAPI DynamicCubemap : public CubemapBase
   {
   public:
      DynamicCubemap(RendererDX11* renderer);
      ~DynamicCubemap();

   private:

   };
}
