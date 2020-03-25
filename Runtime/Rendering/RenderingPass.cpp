#include "Rendering/RenderingPass.h"
#include "Rendering/VertexShaderDX11.h"
#include "Rendering/PixelShaderDX11.h"

namespace Mile
{
   RenderingPass::RenderingPass(RendererDX11* renderer) :
      m_vertexShader(nullptr),
      m_pixelShader(nullptr),
      m_renderer(renderer)
   {
   }

   RenderingPass::~RenderingPass()
   {
      SafeDelete(m_vertexShader);
      SafeDelete(m_pixelShader);
   }

   bool RenderingPass::Init(const String& filePath)
   {
      bool bIsInitialized = m_vertexShader != nullptr || m_pixelShader != nullptr;
      if (m_renderer == nullptr || bIsInitialized)
      {
         return false;
      }

      if (!InitVS(filePath))
      {
         return false;
      }

      if (!InitPS(filePath))
      {
         return false;
      }

      return true;
   }

   bool RenderingPass::Bind(ID3D11DeviceContext& deviceContext)
   {
      if (m_renderer == nullptr)
      {
         return false;
      }

      if (m_vertexShader != nullptr)
      {
         m_vertexShader->Bind(deviceContext);
      }

      if (m_pixelShader != nullptr)
      {
         m_pixelShader->Bind(deviceContext);
      }

      return true;
   }

   void RenderingPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (m_renderer != nullptr)
      {
         if (m_vertexShader != nullptr)
         {
            m_vertexShader->Unbind(deviceContext);
         }

         if (m_pixelShader != nullptr)
         {
            m_pixelShader->Unbind(deviceContext);
         }
      }
   }

   bool RenderingPass::InitVS(const String& filePath)
   {
      if (m_vertexShader != nullptr)
      {
         return false;
      }

      m_vertexShader = new VertexShaderDX11(m_renderer);
      if (!m_vertexShader->Init(filePath))
      {
         SafeDelete(m_vertexShader);
         return false;
      }

      return true;
   }

   bool RenderingPass::InitPS(const String& filePath)
   {
      if (m_pixelShader != nullptr)
      {
         return false;
      }

      m_pixelShader = new PixelShaderDX11(m_renderer);
      if (!m_pixelShader->Init(filePath))
      {
         SafeDelete(m_pixelShader);
         return false;
      }

      return true;
   }
}