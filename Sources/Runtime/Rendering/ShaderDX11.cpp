#include "Rendering/ShaderDX11.h"
#include "Rendering/RendererDX11.h"

namespace Mile
{
   bool ShaderDX11::Compile(const String& shaderPath, EShaderType shaderType)
   {
      if (RenderObject::HasAvailableRenderer())
      {
         RendererDX11* renderer = GetRenderer();
         bool bIsReadyToCompile = !m_bIsCompiled && (renderer != nullptr);
         if (bIsReadyToCompile)
         {
            std::string entryPoint = "Mile";
            std::string target = "_5_0";

            switch (shaderType)
            {
            case EShaderType::VertexShader:
               entryPoint += "VS";
               target = "vs" + target;
               break;
            case EShaderType::HullShader:
               entryPoint += "HS";
               target = "hs" + target;
               break;
            case EShaderType::DomainShader:
               entryPoint += "DS";
               target = "ds" + target;
               break;
            case EShaderType::GeometryShader:
               entryPoint += "GS";
               target = "gs" + target;
               break;
            case EShaderType::PixelShader:
               entryPoint += "PS";
               target = "ps" + target;
               break;
            case EShaderType::ComputeShader:
               entryPoint += "CS";
               target = "cs" + target;
            }

            auto compileFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_OPTIMIZATION_LEVEL3;

#if defined(_DEBUG) | defined(DEBUG)
            compileFlags |= D3D10_SHADER_DEBUG;
#endif

            auto result = D3DCompileFromFile(shaderPath.c_str(),
               nullptr,
               D3D_COMPILE_STANDARD_FILE_INCLUDE,
               entryPoint.c_str(),
               target.c_str(),
               compileFlags, 0,
               &m_blob,
               &m_errorBlob);

            if (!FAILED(result))
            {
               m_bIsCompiled = true;
            }
            else
            {
               /* Failed to compile shader. **/
               /* @TODO: Logging error messages. **/
            }
         }
      }
      return m_bIsCompiled;
   }
}