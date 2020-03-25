#pragma once
#include "Rendering/ShaderDX11.h"

namespace Mile
{
   class SamplerDX11;
   class MEAPI PixelShaderDX11 : public ShaderDX11
   {
   public:
      PixelShaderDX11(RendererDX11* renderer);
      ~PixelShaderDX11();

      virtual bool Init(const String& shaderPath) override;
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      virtual EShaderType GetShaderType() const override { return EShaderType::PixelShader; }

      bool AddSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressModeU, D3D11_TEXTURE_ADDRESS_MODE AddressModeV, D3D11_TEXTURE_ADDRESS_MODE AddressModeW, D3D11_COMPARISON_FUNC compFunc);
      bool AddSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE AddressMode, D3D11_COMPARISON_FUNC compFunc);

   private:
      ID3D11PixelShader* m_shader;
      std::vector<SamplerDX11*> m_samplers;
   };
}
