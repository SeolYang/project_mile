#pragma once
#include "ResourceDX11.h"

namespace Mile
{
   class Texture2dDX11 : public ResourceDX11
   {
   public:
      Texture2dDX11( RendererDX11* renderer ) :
         m_texture( nullptr ),
         m_srv( nullptr ),
         m_mipLevels( 1 ),
         ResourceDX11( renderer )
      {
      }

      bool Init( unsigned int width, unsigned int height, int channels, unsigned char* data, DXGI_FORMAT format );
      bool Bind( unsigned int startSlot, ShaderType shader );

      virtual ID3D11Resource* GetResource( ) override { return m_texture; }
      virtual RenderResourceType GetResourceType( ) const override { return RenderResourceType::RDRT_Texture2D; }
      ID3D11ShaderResourceView* GetSRV( ) { return m_srv; }
      unsigned int GetMipLevels( ) const { return m_mipLevels; }

   private:
      ID3D11Texture2D*           m_texture;
      ID3D11ShaderResourceView*  m_srv;
      unsigned int               m_mipLevels;

   };
}