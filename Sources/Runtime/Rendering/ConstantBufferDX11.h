#pragma once
#include "Rendering/BufferDX11.h"

namespace Mile
{
   class MEAPI ConstantBufferDX11 : public BufferDX11
   {
   public:
      ConstantBufferDX11(RendererDX11* renderer);

      bool Init(unsigned int size);
      template <typename BufferType>
      bool Init()
      {
         return Init(sizeof(BufferType));
      }

      virtual ERenderResourceType GetResourceType() const override { return ERenderResourceType::ConstantBuffer; }

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

      bool BindShaderResourceView(ID3D11DeviceContext& deviceContext, unsigned int bindSlot, EShaderType bindShader) = delete;
      void UnbindShaderResourceView(ID3D11DeviceContext& deviceContext, unsigned int boundSlot, EShaderType boundShader) = delete;
      bool BindUnorderedAccessView(ID3D11DeviceContext& deviceContext, unsigned int bindSlot) = delete;
      void UnbindShaderResourceView(ID3D11DeviceContext& deviceContext, unsigned int boundSlot, EShaderType boundShader) = delete;

      bool Bind(ID3D11DeviceContext& deviceContext, unsigned int bindSlot, EShaderType bindShader);
      void Unbind(ID3D11DeviceContext& deviceContext, unsigned int boundSlot, EShaderType boundShader);

   };
}