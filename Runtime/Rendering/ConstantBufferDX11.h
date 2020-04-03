#pragma once
#include "Rendering/BufferDX11.h"
#include "Rendering/ShaderDX11.h"

namespace Mile
{
   class MEAPI ConstantBufferDX11 : public BufferDX11
   {
   public:
      ConstantBufferDX11(RendererDX11* renderer);
      ~ConstantBufferDX11();

      bool Init(unsigned int size);

      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::ConstantBuffer; }

      virtual void* Map(ID3D11DeviceContext& deviceContext) override;
      virtual bool UnMap(ID3D11DeviceContext& deviceContext) override;

      bool Bind(ID3D11DeviceContext& deviceContext, unsigned int slot, EShaderType shaderType);
      void Unbind(ID3D11DeviceContext& deviceContext);

      FORCEINLINE bool IsBound() const { return m_bIsBound; }
      FORCEINLINE unsigned int GetBoundSlot() const { return m_boundSlot; }
      FORCEINLINE EShaderType GetBoundShaderType() const { return m_boundShader; }

   private:
      bool           m_bIsBound;
      unsigned int   m_boundSlot;
      EShaderType    m_boundShader;

   };
}