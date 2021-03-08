#pragma once
#include "Rendering/ShaderDX11.h"

namespace Mile
{
   class MEAPI ComputeShaderDX11 : public ShaderDX11
   {
   public:
      ComputeShaderDX11(RendererDX11* renderer);
      virtual ~ComputeShaderDX11();

      virtual bool Init(const String& shaderPath) override;
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      virtual EShaderType GetShaderType() const override { return EShaderType::ComputeShader; }

   private:
      ID3D11ComputeShader* m_shader;

   };
}