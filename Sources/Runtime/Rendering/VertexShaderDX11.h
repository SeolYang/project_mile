#pragma once
#include "Rendering/ShaderDX11.h"

namespace Mile
{
   class InputLayoutDX11;
   class MEAPI VertexShaderDX11 : public ShaderDX11
   {
      using InputLayoutPtr = std::unique_ptr<InputLayoutDX11>;

   public:
      VertexShaderDX11(RendererDX11* renderer);
      virtual ~VertexShaderDX11();

      virtual bool Init(const String& shaderPath) override;
      virtual bool Bind(ID3D11DeviceContext& deviceContext) override;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) override;

      virtual EShaderType GetShaderType() const override { return EShaderType::VertexShader; }

   private:
      std::vector<D3D11_INPUT_ELEMENT_DESC> Reflect() const;

   private:
      ID3D11VertexShader* m_shader;
      InputLayoutPtr m_inputLayout;

   };
}