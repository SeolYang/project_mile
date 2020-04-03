#pragma once
#include "Rendering/ResourceDX11.h"

namespace Mile
{
   class Texture2dDX11 : public ResourceDX11
   {
   public:
      Texture2dDX11(RendererDX11* renderer) :
         m_texture(nullptr),
         m_srv(nullptr),
         m_mipLevels(1),
         m_bIsBinded(false),
         m_bindedSlot(0),
         m_bindedShader(EShaderType::PixelShader),
         m_width(0),
         m_height(0),
         ResourceDX11(renderer)
      {
      }

      ~Texture2dDX11();

      bool Init(unsigned int width, unsigned int height, int channels, unsigned char* data, DXGI_FORMAT format);
      bool Init(ID3D11Texture2D* texture);
      bool Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot, EShaderType shader);
      void Unbind(ID3D11DeviceContext& deviceContext);

      virtual ID3D11Resource* GetResource() const override { return m_texture; }
      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::Texture2D; }
      ID3D11ShaderResourceView* GetSRV() { return m_srv; }
      unsigned int GetMipLevels() const { return m_mipLevels; }

      unsigned int GetWidth() const { return m_width; }
      unsigned int GetHeight() const { return m_height; }

   private:
      bool InitSRV(D3D11_TEXTURE2D_DESC desc);

   private:
      ID3D11Texture2D* m_texture;
      ID3D11ShaderResourceView* m_srv;

      bool           m_bIsBinded;
      unsigned int   m_bindedSlot;
      EShaderType    m_bindedShader;

      unsigned int   m_width;
      unsigned int   m_height;
      unsigned int   m_mipLevels;

   };
}