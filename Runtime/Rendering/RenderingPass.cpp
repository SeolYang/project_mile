#include "Rendering/RenderingPass.h"
#include "Rendering/VertexShaderDX11.h"
#include "Rendering/PixelShaderDX11.h"

namespace Mile
{
   RenderingPass::RenderingPass(RendererDX11* renderer) :
      m_vertexShader(nullptr),
      m_pixelShader(nullptr),
      RenderObject(renderer)
   {
   }

   RenderingPass::~RenderingPass()
   {
      SafeDelete(m_vertexShader);
      SafeDelete(m_pixelShader);
   }

   bool RenderingPass::Init(const String& filePath)
   {
      if (IsInitializable())
      {
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

      return false;
   }

   bool RenderingPass::Bind(ID3D11DeviceContext& deviceContext)
   {
      bool bound = false;
      if (RenderObject::IsBindable())
      {
         if (m_vertexShader != nullptr)
         {
            bound = m_vertexShader->Bind(deviceContext);
         }

         if (m_pixelShader != nullptr)
         {
            bound = bound && m_pixelShader->Bind(deviceContext);
         }
      }

      return bound;
   }

   void RenderingPass::Unbind(ID3D11DeviceContext& deviceContext)
   {
      if (RenderObject::IsBindable())
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

      RendererDX11* renderer = GetRenderer();
      m_vertexShader = new VertexShaderDX11(renderer);
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

      RendererDX11* renderer = GetRenderer();
      m_pixelShader = new PixelShaderDX11(renderer);
      if (!m_pixelShader->Init(filePath))
      {
         SafeDelete(m_pixelShader);
         return false;
      }

      return true;
   }
}