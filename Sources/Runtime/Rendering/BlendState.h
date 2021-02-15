#pragma once
#include "Rendering/RenderObject.h"
#include "Math/Vector4.h"

namespace Mile
{
   /**
    * @todo Enumerator pascal convention
    */
   constexpr size_t MAXIMUM_RENDER_TARGETS = 8;
   enum class EBlend : UINT8
   {
      Zero = 1,
      One,
      SrcColor,
      InvSrcColor,
      SrcAlpha,
      InvSrcAlpha,
      DestAlpha,
      InvDestAlpha,
      DestColor,
      InvDestColor,
      SrcAlphaSAT,
      BlendFactor = 14,
      InvBlendFactor,
      Src1Color,
      InvSrc1Color,
      Src1Alpha,
      InvSrc1Alpha
   };

   enum class EBlendOP : UINT8
   {
      Add = 1,
      Subtract,
      RevSubtract,
      Min,
      Max
   };

   enum class EColorWriteEnable : UINT8
   {
      ColorWriteEnableRed = 1,
      ColorWriteEnableGreen = 2,
      ColorWriteEnableBlue = 4,
      ColorWriteEnableAlpha = 8,
      ColorWriteEnableAll = (((ColorWriteEnableRed | ColorWriteEnableGreen) | ColorWriteEnableBlue) | ColorWriteEnableAlpha)
   };

   class MEAPI BlendState : public RenderObject
   {
      struct RenderTargetBlendDesc
      {
         bool     BlendEnable = false;
         EBlend   SrcBlend = EBlend::One;
         EBlend   DestBlend = EBlend::Zero;
         EBlendOP BlendOp = EBlendOP::Add;
         EBlend   SrcBlendAlpha = EBlend::One;
         EBlend   DestBlendAlpha = EBlend::Zero;
         EBlendOP BlendOpAlpha = EBlendOP::Add;
         UINT8    RenderTargetWriteMask = (UINT8)EColorWriteEnable::ColorWriteEnableAll;

         static D3D11_BLEND BlendToD3D11(EBlend blend)
         {
            switch (blend)
            {
            default:
            case EBlend::Zero:
               return D3D11_BLEND::D3D11_BLEND_ZERO;
            case EBlend::One:
               return D3D11_BLEND::D3D11_BLEND_ONE;
            case EBlend::SrcColor:
               return D3D11_BLEND::D3D11_BLEND_SRC_COLOR;
            case EBlend::InvSrcColor:
               return D3D11_BLEND::D3D11_BLEND_INV_SRC_COLOR;
            case EBlend::SrcAlpha:
               return D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
            case EBlend::InvSrcAlpha:
               return D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
            case EBlend::DestAlpha:
               return D3D11_BLEND::D3D11_BLEND_DEST_ALPHA;
            case EBlend::InvDestAlpha:
               return D3D11_BLEND::D3D11_BLEND_INV_DEST_ALPHA;
            case EBlend::DestColor:
               return D3D11_BLEND::D3D11_BLEND_DEST_COLOR;
            case EBlend::InvDestColor:
               return D3D11_BLEND::D3D11_BLEND_INV_DEST_COLOR;
            case EBlend::SrcAlphaSAT:
               return D3D11_BLEND::D3D11_BLEND_SRC_ALPHA_SAT;
            case EBlend::BlendFactor:
               return D3D11_BLEND::D3D11_BLEND_BLEND_FACTOR;
            case EBlend::InvBlendFactor:
               return D3D11_BLEND::D3D11_BLEND_BLEND_FACTOR;
            case EBlend::Src1Color:
               return D3D11_BLEND::D3D11_BLEND_SRC1_COLOR;
            case EBlend::InvSrc1Color:
               return D3D11_BLEND::D3D11_BLEND_INV_SRC1_COLOR;
            case EBlend::Src1Alpha:
               return D3D11_BLEND::D3D11_BLEND_SRC1_ALPHA;
            case EBlend::InvSrc1Alpha:
               return D3D11_BLEND::D3D11_BLEND_INV_SRC1_ALPHA;
            }
         }

         static D3D11_BLEND_OP BlendOpToD3D11(EBlendOP blend)
         {
            switch (blend)
            {
            default:
            case EBlendOP::Add:
               return D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
            case EBlendOP::Subtract:
               return D3D11_BLEND_OP::D3D11_BLEND_OP_SUBTRACT;
            case EBlendOP::Min:
               return D3D11_BLEND_OP::D3D11_BLEND_OP_MIN;
            case EBlendOP::Max:
               return D3D11_BLEND_OP::D3D11_BLEND_OP_MAX;
            case EBlendOP::RevSubtract:
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
      virtual ~BlendState();

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
      ID3D11BlendState* m_blendState;
      bool m_bIsDirty;

      bool					      m_alphaToCoverageEnable;
      bool					      m_independentBlendEnable;
      RenderTargetBlendDesc	m_blendDescs[MAXIMUM_RENDER_TARGETS];
      Vector4 m_blendFactor;
      UINT32 m_sampleMask;

   };
}