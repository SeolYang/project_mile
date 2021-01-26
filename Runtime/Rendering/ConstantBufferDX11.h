#pragma once
#include "Rendering/BufferDX11.h"

namespace Mile
{
   class MEAPI ConstantBufferDX11 : public BufferDX11
   {
   public:
      ConstantBufferDX11(RendererDX11* renderer);
      virtual ~ConstantBufferDX11();

      bool Init(unsigned int size);
      template <typename BufferType>
      bool Init()
      {
         return Init(sizeof(BufferType));
      }

      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::ConstantBuffer; }

      virtual void* Map(ID3D11DeviceContext& deviceContext) override;
      template <typename BufferType>
      BufferType* Map(ID3D11DeviceContext& deviceContext)
      {
         return reinterpret_cast<BufferType*>(Map(deviceContext));
      }

      virtual bool UnMap(ID3D11DeviceContext& deviceContext) override;

      template <typename BufferType>
      bool Update(ID3D11DeviceContext& deviceContext, BufferType buffer)
      {
         auto mappedBuffer = Map<BufferType>(deviceContext);
         if (mappedBuffer != nullptr)
         {
            (*mappedBuffer) = buffer;
         }

         UnMap(deviceContext);
         return false;
      }

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