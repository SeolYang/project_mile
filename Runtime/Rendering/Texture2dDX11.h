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
         m_bIsBinded( false ),
         m_bindedSlot( 0 ),
         m_bindedShader( ShaderType::PixelShader ),
         m_width( 0 ),
         m_height( 0 ),
         ResourceDX11( renderer )
      {
      }

      ~Texture2dDX11( );

      bool Init( unsigned int width, unsigned int height, int channels, unsigned char* data, DXGI_FORMAT format );
      bool Init( ID3D11Texture2D* texture );
      bool Bind( unsigned int startSlot, ShaderType shader );
      void Unbind( );

      virtual ID3D11Resource* GetResource( ) override { return m_texture; }
      virtual RenderResourceType GetResourceType( ) const override { return RenderResourceType::RDRT_Texture2D; }
      ID3D11ShaderResourceView* GetSRV( ) { return m_srv; }
      unsigned int GetMipLevels( ) const { return m_mipLevels; }

      unsigned int GetWidth( ) const { return m_width; }
      unsigned int GetHeight( ) const { return m_height; }

   private:
      bool InitSRV( D3D11_TEXTURE2D_DESC desc );

   private:
      ID3D11Texture2D*           m_texture;
      ID3D11ShaderResourceView*  m_srv;
      unsigned int               m_mipLevels;

      bool                       m_bIsBinded;
      unsigned int               m_bindedSlot;
      ShaderType                 m_bindedShader;

      unsigned int               m_width;
      unsigned int               m_height;

   };
}