#pragma once
#include "Core/SubSystem.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"

#pragma warning(push, 0)
#include <d3d11.h>
#include <d3dcompiler.h>
#pragma warning(pop)

#define SAFE_SHADER_RESOURCE_VIEW_BIND(RESOURCE, DEVICE_CONTEXT, BIND_SLOT, BIND_SHADER) if (RESOURCE != nullptr) { RESOURCE->BindShaderResourceView(DEVICE_CONTEXT, BIND_SLOT, BIND_SHADER); }
#define SAFE_SHADER_RESOURCE_VIEW_UNBIND(RESOURCE, DEVICE_CONTEXT, BOUND_SLOT, BOUND_SHADER) if (RESOURCE != nullptr) { RESOURCE->UnbindShaderResourceView(DEVICE_CONTEXT, BOUND_SLOT, BOUND_SHADER); }

#define SAFE_EXECUTE_CMDLIST(DEVICE_CONTEXT_PTR, CMD_LIST, RESTORE_CONTEXT_STATE) if(DEVICE_CONTEXT_PTR != nullptr && CMD_LIST != nullptr) { DEVICE_CONTEXT_PTR->ExecuteCommandList(CMD_LIST, RESTORE_CONTEXT_STATE); }

#define EDITOR_GAME_VIEW_RENDER_TEXTURE TEXT("Editor/GameViewRenderTexture")
#define DEFINE_CONSTANT_BUFFER(BUFFER_NAME) struct MEAPI alignas(16) BUFFER_NAME

namespace Mile
{
   constexpr unsigned int CUBE_FACES = 6;

   enum class MEAPI ECullMode
   {
      None,
      Front,
      Back
   };

   enum class MEAPI EWindingOrder
   {
      CCW,
      CW
   };

   enum class MEAPI EShaderType
   {
      VertexShader,
      HullShader,
      DomainShader,
      GeometryShader,
      PixelShader,
      ComputeShader,
      EnumSize
   };

   enum class MEAPI EStaticMeshType
   {
      External = 0,
      Cube = 1,
      Quad = 2,
   };

   /**
    * Equivalent to D3D11_PRIMITIVE_TOPOLOGY
    */
   enum class MEAPI EPrimitiveTopology : UINT32
   {
      PRIMITIVE_TOPOLOGY_UNDEFINED = 0,
      PRIMITIVE_TOPOLOGY_POINTLIST = 1,
      PRIMITIVE_TOPOLOGY_LINELIST = 2,
      PRIMITIVE_TOPOLOGY_LINESTRIP = 3,
      PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
      PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 5,
      PRIMITIVE_TOPOLOGY_LINELIST_ADJ = 10,
      PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ = 11,
      PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ = 12,
      PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ = 13,
      PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST = 33,
      PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST = 34,
      PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST = 35,
      PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST = 36,
      PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST = 37,
      PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST = 38,
      PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST = 39,
      PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST = 40,
      PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST = 41,
      PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST = 42,
      PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST = 43,
      PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST = 44,
      PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST = 45,
      PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST = 46,
      PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST = 47,
      PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST = 48,
      PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST = 49,
      PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST = 50,
      PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST = 51,
      PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST = 52,
      PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST = 53,
      PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST = 54,
      PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST = 55,
      PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST = 56,
      PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST = 57,
      PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST = 58,
      PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST = 59,
      PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST = 60,
      PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST = 61,
      PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST = 62,
      PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST = 63,
      PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST = 64
   };

   static inline D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopologyToD3D11(EPrimitiveTopology topology)
   {
      return static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology);
   }

   /**
    * Equivalent to DXGI_FORMAT
    */
   enum class MEAPI EColorFormat : UINT32
   {
      UNKNOWN = 0,
      R32G32B32A32_TYPELESS = 1,
      R32G32B32A32_FLOAT = 2,
      R32G32B32A32_UINT = 3,
      R32G32B32A32_SINT = 4,
      R32G32B32_TYPELESS = 5,
      R32G32B32_FLOAT = 6,
      R32G32B32_UINT = 7,
      R32G32B32_SINT = 8,
      R16G16B16A16_TYPELESS = 9,
      R16G16B16A16_FLOAT = 10,
      R16G16B16A16_UNORM = 11,
      R16G16B16A16_UINT = 12,
      R16G16B16A16_SNORM = 13,
      R16G16B16A16_SINT = 14,
      R32G32_TYPELESS = 15,
      R32G32_FLOAT = 16,
      R32G32_UINT = 17,
      R32G32_SINT = 18,
      R32G8X24_TYPELESS = 19,
      D32_FLOAT_S8X24_UINT = 20,
      R32_FLOAT_X8X24_TYPELESS = 21,
      X32_TYPELESS_G8X24_UINT = 22,
      R10G10B10A2_TYPELESS = 23,
      R10G10B10A2_UNORM = 24,
      R10G10B10A2_UINT = 25,
      R11G11B10_FLOAT = 26,
      R8G8B8A8_TYPELESS = 27,
      R8G8B8A8_UNORM = 28,
      R8G8B8A8_UNORM_SRGB = 29,
      R8G8B8A8_UINT = 30,
      R8G8B8A8_SNORM = 31,
      R8G8B8A8_SINT = 32,
      R16G16_TYPELESS = 33,
      R16G16_FLOAT = 34,
      R16G16_UNORM = 35,
      R16G16_UINT = 36,
      R16G16_SNORM = 37,
      R16G16_SINT = 38,
      R32_TYPELESS = 39,
      D32_FLOAT = 40,
      R32_FLOAT = 41,
      R32_UINT = 42,
      R32_SINT = 43,
      R24G8_TYPELESS = 44,
      D24_UNORM_S8_UINT = 45,
      R24_UNORM_X8_TYPELESS = 46,
      X24_TYPELESS_G8_UINT = 47,
      R8G8_TYPELESS = 48,
      R8G8_UNORM = 49,
      R8G8_UINT = 50,
      R8G8_SNORM = 51,
      R8G8_SINT = 52,
      R16_TYPELESS = 53,
      R16_FLOAT = 54,
      D16_UNORM = 55,
      R16_UNORM = 56,
      R16_UINT = 57,
      R16_SNORM = 58,
      R16_SINT = 59,
      R8_TYPELESS = 60,
      R8_UNORM = 61,
      R8_UINT = 62,
      R8_SNORM = 63,
      R8_SINT = 64,
      A8_UNORM = 65,
      R1_UNORM = 66,
      R9G9B9E5_SHAREDEXP = 67,
      R8G8_B8G8_UNORM = 68,
      G8R8_G8B8_UNORM = 69,
      BC1_TYPELESS = 70,
      BC1_UNORM = 71,
      BC1_UNORM_SRGB = 72,
      BC2_TYPELESS = 73,
      BC2_UNORM = 74,
      BC2_UNORM_SRGB = 75,
      BC3_TYPELESS = 76,
      BC3_UNORM = 77,
      BC3_UNORM_SRGB = 78,
      BC4_TYPELESS = 79,
      BC4_UNORM = 80,
      BC4_SNORM = 81,
      BC5_TYPELESS = 82,
      BC5_UNORM = 83,
      BC5_SNORM = 84,
      B5G6R5_UNORM = 85,
      B5G5R5A1_UNORM = 86,
      B8G8R8A8_UNORM = 87,
      B8G8R8X8_UNORM = 88,
      R10G10B10_XR_BIAS_A2_UNORM = 89,
      B8G8R8A8_TYPELESS = 90,
      B8G8R8A8_UNORM_SRGB = 91,
      B8G8R8X8_TYPELESS = 92,
      B8G8R8X8_UNORM_SRGB = 93,
      BC6H_TYPELESS = 94,
      BC6H_UF16 = 95,
      BC6H_SF16 = 96,
      BC7_TYPELESS = 97,
      BC7_UNORM = 98,
      BC7_UNORM_SRGB = 99,
      AYUV = 100,
      Y410 = 101,
      Y416 = 102,
      NV12 = 103,
      P010 = 104,
      P016 = 105,
      _420_OPAQUE = 106,
      YUY2 = 107,
      Y210 = 108,
      Y216 = 109,
      NV11 = 110,
      AI44 = 111,
      IA44 = 112,
      P8 = 113,
      A8P8 = 114,
      B4G4R4A4_UNORM = 115,
      P208 = 130,
      V208 = 131,
      V408 = 132,
      FORCE_UINT = 0xffffffff
   };

   constexpr size_t MAXIMUM_RENDER_TARGETS = 8;
   enum class MEAPI EBlend : UINT8
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

   enum class MEAPI EBlendOP : UINT8
   {
      Add = 1,
      Subtract,
      RevSubtract,
      Min,
      Max
   };

   enum class MEAPI EColorWriteEnable : UINT8
   {
      ColorWriteEnableRed = 1,
      ColorWriteEnableGreen = 2,
      ColorWriteEnableBlue = 4,
      ColorWriteEnableAlpha = 8,
      ColorWriteEnableAll = (((ColorWriteEnableRed | ColorWriteEnableGreen) | ColorWriteEnableBlue) | ColorWriteEnableAlpha)
   };

   struct MEAPI RenderTargetBlendDesc
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
         return D3D11_RENDER_TARGET_BLEND_DESC{
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
            RenderTargetWriteMask == lhs.RenderTargetWriteMask);
      }

      bool operator!=(const RenderTargetBlendDesc& lhs)
      {
         return !((*this) == lhs);
      }
   };

   class Material;
   class MeshRenderComponent;
   using MaterialMap = std::map<Material*, std::vector<MeshRenderComponent*>>;

   static inline DXGI_FORMAT ColorFormatToDXGIFormat(EColorFormat format)
   {
      return static_cast<DXGI_FORMAT>(format);
   }

   static inline Vector2 FindResolutionWithAspectRatio(float currentWidth, float currentHeight, float targetAspectRatio)
   {
      float width = currentWidth;
      float height = (width / targetAspectRatio + 0.5f);

      if (height > currentHeight)
      {
         height = currentHeight;
         width = (height * targetAspectRatio + 0.5f);
      }

      return Vector2(width, height);
   }
}
