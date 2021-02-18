#pragma once
#include "Rendering/ShaderDX11.h"

namespace Mile
{
   class SamplerDX11;
   class MEAPI PixelShaderDX11 : public ShaderDX11
   {
   public:
      PixelShaderDX11(RendererDX11* renderer);
      virtual ~PixelShaderDX11();

      virtual bool Init(const String& shaderPath) override;
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      virtual EShaderType GetShaderType() const override { return EShaderType::PixelShader; }

   private:
      ID3D11PixelShader* m_shader;

   };
}
