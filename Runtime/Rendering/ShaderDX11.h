#pragma once
#include "Rendering/RenderObject.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   class MEAPI ShaderDX11 : public RenderObject
   {
   public:
      ShaderDX11(RendererDX11* renderer) :
         m_blob(nullptr),
         m_errorBlob(nullptr),
         m_bIsCompiled(false),
         RenderObject(renderer)
      {
      }

      virtual ~ShaderDX11()
      {
         SafeRelease(m_blob);
         SafeRelease(m_errorBlob);
      }

      virtual bool Init(const String& shaderPath) = 0;
      virtual bool Bind(ID3D11DeviceContext& deviceContext) = 0;
      virtual void Unbind(ID3D11DeviceContext& deviceContext) = 0;

      ID3D10Blob* GetBlob() { return m_blob; }

      bool IsCompiled() const { return m_bIsCompiled; }
      virtual EShaderType GetShaderType() const = 0;

   protected:
      virtual bool Compile(const String& shaderPath, EShaderType shaderType) final;

   protected:
      ID3D10Blob* m_blob;
      ID3D10Blob* m_errorBlob;
      bool        m_bIsCompiled;

   };
}