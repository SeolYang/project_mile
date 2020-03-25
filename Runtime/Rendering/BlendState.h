#pragma once
#include "Rendering/RenderingCore.h"
#include "Math/Vector4.h"

namespace Mile
{
   constexpr size_t MAXIMUM_RENDER_TARGETS = 8;
   enum class EBlend : UINT8
   {
      ZERO = 1,
      ONE,
      SRC_COLOR,
      INV_SRC_COLOR,
      SRC_ALPHA,
      INV_SRC_ALPHA,
      DEST_ALPHA,
      INV_DEST_ALPHA,
      DEST_COLOR,
      INV_DEST_COLOR,
      SRC_ALPHA_SAT,
      BLEND_FACTOR = 14,
      INV_BLEND_FACTOR,
      SRC1_COLOR,
      INV_SRC1_COLOR,
      SRC1_ALPHA,
      INV_SRC1_ALPHA
   };

   enum class EBlendOP : UINT8
   {
      ADD = 1,
      SUBTRACT,
      REV_SUBTRACT,
      MIN,
      MAX
   };

   enum class EColorWriteEnable : UINT8
   {
      ColorWriteEnableRed = 1,
      ColorWriteEnableGreen = 2,
      ColorWriteEnableBlue = 4,
      ColorWriteEnableAlpha = 8,
      ColorWriteEnalbeAll = (((ColorWriteEnableRed | ColorWriteEnableGreen) | ColorWriteEnableBlue) | ColorWriteEnableAlpha)
   };

   class MEAPI BlendState
   {
      struct RenderTargetBlendDesc
      {
         bool     BlendEnable = false;
         EBlend   SrcBlend = EBlend::ONE;
         EBlend   DestBlend = EBlend::ZERO;
         EBlendOP BlendOp = EBlendOP::ADD;
         EBlend   SrcBlendAlpha = EBlend::ONE;
         EBlend   DestBlendAlpha = EBlend::ZERO;
         EBlendOP BlendOpAlpha = EBlendOP::ADD;
         UINT8    RenderTargetWriteMask = (UINT8)EColorWriteEnable::ColorWriteEnalbeAll;

         D3D11_RENDER_TARGET_BLEND_DESC ToD3D11() const
         {
            return {
               BlendEnable,
               static_cast<UINT8>(SrcBlend),
               static_cast<UINT8>(DestBlend),
               static_cast<UINT8>(BlendOp),
               static_cast<UINT8>(SrcBlendAlpha),
               static_cast<UINT8>(DestBlendAlpha),
               static_cast<UINT8>(BlendOpAlpha),
               RenderTargetWriteMask
            };
         }
      };

   public:
      BlendState(class RendererDX11* renderer);
      ~BlendState();

      bool Init();
      bool Bind(ID3D11DeviceContext& deviceContext);

      void SetRenderTargetBlendState(RenderTargetBlendDesc desc, size_t renderTargetIndex = 0);
      RenderTargetBlendDesc GetRenderTargetBlendState() const;

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
      RendererDX11* m_renderer;
      ID3D11BlendState* m_blendState;
      bool m_bIsDirty;

      bool					      m_alphaToCoverageEnable;
      bool					      m_independentBlendEnable;
      RenderTargetBlendDesc	m_blendDescs[MAXIMUM_RENDER_TARGETS];
      Vector4 m_blendFactor;
      UINT32 m_sampleMask;

   };
}