#pragma once
#include "Rendering/BufferDX11.h"
#include "Rendering/ShaderDX11.h"

namespace Mile
{
   class MEAPI ConstantBufferDX11 : public BufferDX11
   {
   public:
      ConstantBufferDX11(RendererDX11* renderer) :
         m_bIsBinded(false),
         m_bindedSlot(0),
         m_bindedShader(EShaderType::PixelShader),
         BufferDX11(renderer)
      {
      }

      ~ConstantBufferDX11()
      {
         this->UnMapImmediately();
      }

      bool Init(unsigned int size);

      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::ConstantBuffer; }

      virtual void* Map(ID3D11DeviceContext& deviceContext) override;
      virtual bool UnMap(ID3D11DeviceContext& deviceContext) override;

      bool Bind(ID3D11DeviceContext& deviceContext, unsigned int slot, EShaderType shaderType);
      void Unbind(ID3D11DeviceContext& deviceContext);

   private:
      bool           m_bIsBinded;
      unsigned int   m_bindedSlot;
      EShaderType    m_bindedShader;

   };
}