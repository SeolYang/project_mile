#pragma once
#include "Rendering/Texture2DBaseDX11.h"

namespace Mile
{
   class Texture2dDX11 : public Texture2DBaseDX11
   {
   public:
      Texture2dDX11(RendererDX11* renderer);
      ~Texture2dDX11();

      bool Init(unsigned int width, unsigned int height, int channels, unsigned char* data, DXGI_FORMAT format);
      bool Init(ID3D11Texture2D* texture);

   private:
      virtual bool InitSRV(D3D11_TEXTURE2D_DESC desc) override;

   };
}