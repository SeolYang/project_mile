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
      ColorWriteEnableAll = (((ColorWriteEnableRed | ColorWriteEnableGreen) | ColorWriteEnableBlue) | ColorWriteEnableAlpha)
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
         UINT8    RenderTargetWriteMask = (UINT8)EColorWriteEnable::ColorWriteEnableAll;

         static D3D11_BLEND BlendToD3D11(EBlend blend)
         {
            switch (blend)
            {
            default:
            case EBlend::ZERO:
               return D3D11_BLEND::D3D11_BLEND_ZERO;
            case EBlend::ONE:
               return D3D11_BLEND::D3D11_BLEND_ONE;
            case EBlend::SRC_COLOR:
               return D3D11_BLEND::D3D11_BLEND_SRC_COLOR;
            case EBlend::INV_SRC_COLOR:
               return D3D11_BLEND::D3D11_BLEND_INV_SRC_COLOR;
            case EBlend::SRC_ALPHA:
               return D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
            case EBlend::INV_SRC_ALPHA:
               return D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
            case EBlend::DEST_ALPHA:
               return D3D11_BLEND::D3D11_BLEND_DEST_ALPHA;
            case EBlend::INV_DEST_ALPHA:
               return D3D11_BLEND::D3D11_BLEND_INV_DEST_ALPHA;
            case EBlend::DEST_COLOR:
               return D3D11_BLEND::D3D11_BLEND_DEST_COLOR;
            case EBlend::INV_DEST_COLOR:
               return D3D11_BLEND::D3D11_BLEND_INV_DEST_COLOR;
            case EBlend::SRC_ALPHA_SAT:
               return D3D11_BLEND::D3D11_BLEND_SRC_ALPHA_SAT;
            case EBlend::BLEND_FACTOR:
               return D3D11_BLEND::D3D11_BLEND_BLEND_FACTOR;
            case EBlend::INV_BLEND_FACTOR:
               return D3D11_BLEND::D3D11_BLEND_BLEND_FACTOR;
            case EBlend::SRC1_COLOR:
               return D3D11_BLEND::D3D11_BLEND_SRC1_COLOR;
            case EBlend::INV_SRC1_COLOR:
               return D3D11_BLEND::D3D11_BLEND_INV_SRC1_COLOR;
            case EBlend::SRC1_ALPHA:
               return D3D11_BLEND::D3D11_BLEND_SRC1_ALPHA;
            case EBlend::INV_SRC1_ALPHA:
               return D3D11_BLEND::D3D11_BLEND_INV_SRC1_ALPHA;
            }
         }

         static D3D11_BLEND_OP BlendOpToD3D11(EBlendOP blend)
         {
            switch (blend)
            {
            default:
            case EBlendOP::ADD:
               return D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
            case EBlendOP::SUBTRACT:
               return D3D11_BLEND_OP::D3D11_BLEND_OP_SUBTRACT;
            case EBlendOP::MIN:
               return D3D11_BLEND_OP::D3D11_BLEND_OP_MIN;
            case EBlendOP::MAX:
               return D3D11_BLEND_OP::D3D11_BLEND_OP_MAX;
            case EBlendOP::REV_SUBTRACT:
               return D3D11_BLEND_OP::D3D11_BLEND_OP_REV_SUBTRACT;
            }
         }

         D3D11_RENDER_TARGET_BLEND_DESC ToD3D11() const
         {
            return D3D11_RENDER_TARGET_BLEND_DESC {
               BlendEnable,
               BlendToD3D11(SrcBlend),
               BlendToD3D11(DestBlend),
               BlendOpToD3D11(BlendOp),
               BlendToD3D11(SrcBlendAlpha),
               BlendToD3D11(DestBlendAlpha),
               BlendOpToD3D11(BlendOpAlpha),
               RenderTargetWriteMask
            };
         }

         bool operator==(const RenderTargetBlendDesc& lhs)
         {
            return (
               BlendEnable == lhs.BlendEnable &&
               SrcBlend == lhs.SrcBlend &&
               DestBlend == lhs.DestBlend &&
               BlendOp == lhs.BlendOp &&
               SrcBlendAlpha == lhs.SrcBlendAlpha &&
               DestBlendAlpha == lhs.DestBlendAlpha &&
               BlendOpAlpha == lhs.BlendOpAlpha &&
               RenderTargetWriteMask == lhs.RenderTargetWriteMask );
         }

         bool operator!=(const RenderTargetBlendDesc& lhs)
         {
            return !((*this) == lhs);
         }
      };

   public:
      BlendState(class RendererDX11* renderer);
      ~BlendState();

      bool Init();
      bool Bind(ID3D11DeviceContext& deviceContext);

      void SetRenderTargetBlendState(RenderTargetBlendDesc desc, size_t renderTargetIndex = 0);
      RenderTargetBlendDesc GetRenderTargetBlendState(size_t renderTargetIndex) const { return m_blendDescs[renderTargetIndex]; }

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