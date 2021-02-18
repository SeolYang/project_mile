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

      bool Bind(ID3D11DeviceContext& deviceContext, unsigned int bindSlot, EShaderType bindShader);
      void Unbind(ID3D11DeviceContext& deviceContext, unsigned int boundSlot, EShaderType boundShader);

   };
}