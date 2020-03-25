#pragma once
#include "Rendering/RenderingCore.h"

namespace Mile
{
   enum class EBlend
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

   enum class EBlendOP
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
      };

   public:
      BlendState(class RendererDX11* renderer);
      ~BlendState();

   private:
      bool					      m_alphaToCoverageEnable;
      bool					      m_independentBlendEnable;
      RenderTargetBlendDesc	m_renderTargets[8];

   };
}