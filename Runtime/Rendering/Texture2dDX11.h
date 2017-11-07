#pragma once
#include "ResourceDX11.h"

namespace Mile
{
   class Texture2dDX11 : public ResourceDX11
   {
   public:
      Texture2dDX11( RendererDX11* renderer ) :
         m_texture( nullptr ),
         m_mipLevels( 1 ),
         ResourceDX11( renderer )
      {
      }

      bool Init( unsigned int width, unsigned int height, int channels, unsigned char* data, DXGI_FORMAT format );

      ID3D11Texture2D* GetTexture( ) { return m_texture; }
      unsigned int GetMipLevels( ) const { return m_mipLevels; }

   private:
      ID3D11Texture2D*   m_texture;
      unsigned int       m_mipLevels;

   };
}