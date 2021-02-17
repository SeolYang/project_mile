#pragma once
#include "Rendering/RenderObject.h"
#include "Math/Vector4.h"

namespace Mile
{
   class MEAPI BlendState : public RenderObject
   {

   public:
      BlendState(class RendererDX11* renderer);
      virtual ~BlendState();

      bool Init();
      bool Bind(ID3D11DeviceContext& deviceContext);

      void SetRenderTargetBlendState(RenderTargetBlendDesc desc, size_t renderTargetIndex = 0);
      RenderTargetBlendDesc GetRenderTargetBlendState(size_t renderTargetIndex) const { return m_blendDescs[renderTargetIndex]; }

      void SetRenderTargetBlendStates(const RenderTargetBlendDesc descs[MAXIMUM_RENDER_TARGETS]) 
      { 
         for (size_t idx = 0; idx < MAXIMUM_RENDER_TARGETS; ++idx)
         {
            m_blendDescs[idx] = descs[idx];
         }
      }

      void SetAlphaToCoverageEnable(bool bIsEnable);
      bool IsAlphaToCoverageEnabled() const { return m_alphaToCoverageEnable; }

      void SetIndependentBlendEnable(bool bIsEnable);
      bool IsIndependentBlendEnable() const { return m_independentBlendEnable; }

      void SetBlendFactor(Vector4 blendFactor) { m_blendFactor = blendFactor; }
      Vector4 GetBlendFactor() const { return m_blendFactor; }

      void SetSampleMask(UINT32 sampleMask) { m_sampleMask = sampleMask; }
      UINT32 GetSampleMask() const { return m_sampleMask; }

      ID3D11BlendState* GetBlendState() const { return m_blendState; }

   private:
      ID3D11BlendState* m_blendState;
      bool m_bIsDirty;

      bool					      m_alphaToCoverageEnable;
      bool					      m_independentBlendEnable;
      RenderTargetBlendDesc	m_blendDescs[MAXIMUM_RENDER_TARGETS];
      Vector4 m_blendFactor;
      UINT32 m_sampleMask;

   };
}