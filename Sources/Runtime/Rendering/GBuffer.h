#pragma once
#include "Rendering/RenderObject.h"
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileGBuffer, Log);

   class RenderTargetDX11;
   class DepthStencilBufferDX11;
   class BlendState;
   class MEAPI GBuffer : public RenderObject
   {
   public:
      GBuffer(RendererDX11* renderer);
      virtual ~GBuffer();

      bool Init(unsigned int width, unsigned int height);

      DepthStencilBufferDX11* GetDepthStencilBufferDX11() const { return this->m_depthStencilBuffer; }

      bool BindAsRenderTarget(ID3D11DeviceContext& deviceContext, bool clearRenderTargets = true, bool clearDepthStencil = true);
      bool BindAsShaderResource(ID3D11DeviceContext& deviceContext, unsigned int bindSlot, EShaderType bindShader, bool bBindDepthStencil = false);
      void UnbindRenderTarget(ID3D11DeviceContext& deviceContext);
      void UnbindShaderResource(ID3D11DeviceContext& deviceContext, unsigned int boundSlot, EShaderType boundShader, bool bBoundDepthStencil = false);

      void UnbindDepthBuffer(ID3D11DeviceContext& deviceContext);

      RenderTargetDX11* GetPositionBuffer() const { return m_positionBuffer; }
      RenderTargetDX11* GetAlbedoBuffer() const { return m_albedoBuffer; }
      RenderTargetDX11* GetEmissiveAOBuffer() const { return m_emissiveAOBuffer; }
      RenderTargetDX11* GetNormalBuffer() const { return m_normalBuffer; }

   private:
      DepthStencilBufferDX11* m_depthStencilBuffer;
      RenderTargetDX11* m_positionBuffer;
      RenderTargetDX11* m_albedoBuffer;
      RenderTargetDX11* m_emissiveAOBuffer;
      RenderTargetDX11* m_normalBuffer;
      RenderTargetDX11* m_extraComponents;
      BlendState*       m_blendState;

      RenderTargetDX11* m_tempRenderTarget;

   };
}