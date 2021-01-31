#pragma once
#include "Rendering/ResourceDX11.h"

namespace Mile
{
   class MEAPI Texture2DBaseDX11 : public ResourceDX11
   {
   public:
      Texture2DBaseDX11(RendererDX11* renderer);
      virtual ~Texture2DBaseDX11();

      virtual ID3D11Resource* GetResource() const override { return m_texture; }
      FORCEINLINE ID3D11ShaderResourceView* GetSRV() const { return m_srv; }

      bool Bind(ID3D11DeviceContext& deviceContext, unsigned int startSlot, EShaderType shader);
      void Unbind(ID3D11DeviceContext& deviceContext);

      FORCEINLINE unsigned int GetWidth() const { return m_width; }
      FORCEINLINE unsigned int GetHeight() const { return m_height; }
      FORCEINLINE unsigned int GetMipLevels() const { return m_mipLevels; }
      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::Texture2D; }

      FORCEINLINE bool IsBoundAsShaderResource() const { return m_bIsBoundAsShaderResource; }
      FORCEINLINE EShaderType GetBoundShaderType() const { return m_boundShader; }
      FORCEINLINE unsigned int GetBoundSlot() const { return m_boundSlot; }

      void GenerateMips(ID3D11DeviceContext& deviceContext);

   protected:
      bool InitSRV(D3D11_TEXTURE2D_DESC desc, bool bIsCubemap = false);

   protected:
      ID3D11Texture2D* m_texture;
      ID3D11ShaderResourceView* m_srv;

      bool           m_bIsBoundAsShaderResource;
      unsigned int   m_boundSlot;
      EShaderType    m_boundShader;

      unsigned int   m_width;
      unsigned int   m_height;
      unsigned int   m_mipLevels;

   };
}