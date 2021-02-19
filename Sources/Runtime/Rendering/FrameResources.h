#pragma once
#include "Rendering/RenderingCore.h"
#include "Elaina/FrameResource.h"

namespace Mile
{
   class CameraComponent;
   using CameraRef = CameraComponent*;
   struct CameraRefDescriptor
   {
      CameraRef Reference;
   };
   using CameraRefResource = Elaina::FrameResource<CameraRefDescriptor, CameraRef>;

   class World;
   struct WorldDescriptor
   {
      World* TargetWorld = nullptr;
   };

   class LightComponent;
   using Lights = std::vector<LightComponent*>;
   using LightsDataResource = Elaina::FrameResource<WorldDescriptor, Lights>;

   class MeshRenderComponent;
   using Meshes = std::vector<MeshRenderComponent*>;
   using MeshesDataResource = Elaina::FrameResource<WorldDescriptor, Meshes>;

   using MaterialMapResource = Elaina::FrameResource<WorldDescriptor, MaterialMap>;

   class RenderTargetDX11;
   class RendererDX11;
   class DepthStencilBufferDX11;
   using RenderTargetRef = RenderTargetDX11*;
   struct RenderTargetDescriptor
   {
      RendererDX11* Renderer = nullptr;
      RenderTargetRef* ResolutionReference = nullptr;
      unsigned int Width = 1920;
      unsigned int Height = 1080;
      RenderTargetRef* FormatReference = nullptr;
      EColorFormat Format = EColorFormat::R8G8B8A8_UNORM;
      ID3D11RenderTargetView* RenderTargetView = nullptr;
      DepthStencilBufferDX11* DepthStencilBuffer = nullptr;
   };
   using RenderTargetResource = Elaina::FrameResource<RenderTargetDescriptor, RenderTargetDX11>;

   struct RenderTargetRefDescriptor
   {
      RenderTargetRef Reference;
   };
   using RenderTargetRefResource = Elaina::FrameResource<RenderTargetRefDescriptor, RenderTargetRef>;

   struct DepthStencilBufferDescriptor
   {
      RendererDX11* Renderer;
      unsigned int Width = 1920;
      unsigned int Height = 1080;
      bool bStencilEnable = false;
   };
   using DepthStencilBufferResource = Elaina::FrameResource<DepthStencilBufferDescriptor, DepthStencilBufferDX11>;

   using DepthStencilBufferRef = DepthStencilBufferDX11*;
   struct DepthStencilBufferRefDescriptor
   {
      DepthStencilBufferRef Reference;
   };
   using DepthStencilBufferRefResource = Elaina::FrameResource<DepthStencilBufferRefDescriptor, DepthStencilBufferRef>;

   class VertexShaderDX11;
   class PixelShaderDX11;
   struct ShaderDescriptor
   {
      RendererDX11* Renderer = nullptr;
      String FilePath = TEXT("");
   };
   using VertexShaderResource = Elaina::FrameResource<ShaderDescriptor, VertexShaderDX11>;
   using PixelShaderResource = Elaina::FrameResource<ShaderDescriptor, PixelShaderDX11>;

   class SamplerDX11;
   struct SamplerDescriptor
   {
      RendererDX11* Renderer = nullptr;
      D3D11_FILTER Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
      D3D11_TEXTURE_ADDRESS_MODE AddressModeU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
      D3D11_TEXTURE_ADDRESS_MODE AddressModeV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
      D3D11_TEXTURE_ADDRESS_MODE AddressModeW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
      D3D11_COMPARISON_FUNC CompFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
   };
   using SamplerResource = Elaina::FrameResource<SamplerDescriptor, SamplerDX11>;

   class ConstantBufferDX11;
   struct ConstantBufferDescriptor
   {
      RendererDX11* Renderer = nullptr;
      size_t Size = 0;
   };
   using ConstantBufferResource = Elaina::FrameResource<ConstantBufferDescriptor, ConstantBufferDX11>;

   class GBuffer;
   struct GBufferDescriptor
   {
      RendererDX11* Renderer = nullptr;
      RenderTargetRef* OutputRenderTargetReference = nullptr;
      unsigned int Width = 1920;
      unsigned int Height = 1080;
   };
   using GBufferResource = Elaina::FrameResource<GBufferDescriptor, GBuffer>;

   using GBufferRef = GBuffer*;
   struct GBufferRefDescriptor
   {
      GBufferRef Reference;
   };
   using GBufferRefResource = Elaina::FrameResource<GBufferRefDescriptor, GBufferRef>;

   class Viewport;
   struct ViewportDescriptor
   {
      RendererDX11* Renderer = nullptr;
      RenderTargetRef* OutputRenderTargetReference = nullptr;
      float Width = 1920.0f;
      float Height = 1080.0f;
      float MinDepth = 0.0f;
      float MaxDepth = 1.0f;
      float TopLeftX = 0.0f;
      float TopLeftY = 0.0f;
   };
   using ViewportResource = Elaina::FrameResource<ViewportDescriptor, Viewport>;

   class RasterizerState;
   struct RasterizerStateDescriptor
   {
      RendererDX11* Renderer = nullptr;
      bool bIsWireframe = false;
      ECullMode CullMode = ECullMode::Back;
      EWindingOrder WindingOrder = EWindingOrder::CW;
      int DepthBias = 0;
      float SlopeScaledDepthBias = 0.0f;
      float DepthBiasClamp = 0.0f;
      bool bIsDepthClipEnable = true;
   };
   using RasterizerStateResource = Elaina::FrameResource<RasterizerStateDescriptor, RasterizerState>;

   class DepthStencilState;
   struct DepthStencilStateDescriptor
   {
      RendererDX11* Renderer = nullptr;
      bool bDepthEnable = true;
      D3D11_DEPTH_WRITE_MASK DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      D3D11_COMPARISON_FUNC DepthFunc = D3D11_COMPARISON_LESS;
      bool bStencilEnable = false;
      UINT8 StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
      UINT8 StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
      D3D11_DEPTH_STENCILOP_DESC FrontFace{ D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
      D3D11_DEPTH_STENCILOP_DESC BackFace{ D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
   };
   using DepthStencilStateResource = Elaina::FrameResource<DepthStencilStateDescriptor, DepthStencilState>;

   class BlendState;
   struct BlendStateDescriptor
   {
      RendererDX11* Renderer = nullptr;
      bool bAlphaToConverageEnable = false;
      bool bIndependentBlendEnable = false;
      RenderTargetBlendDesc BlendDescs[MAXIMUM_RENDER_TARGETS];
      Vector4 BlendFactor = Vector4(1.f, 1.f, 1.f, 1.f);
      UINT32 SampleMask = 0xffffffff;
   };
   using BlendStateResource = Elaina::FrameResource<BlendStateDescriptor, BlendState>;

   class Texture2D;
   /** Resource Texture */
   using Texture2DRef = Texture2D*;
   struct Texture2DRefDescriptor
   {
      String ResourcePath = TEXT("");
      Texture2DRef Reference = nullptr;
   };
   using Texture2DRefResource = Elaina::FrameResource<Texture2DRefDescriptor, Texture2DRef>;

   using BoolRef = bool*;
   struct BoolRefDescriptor
   {
      BoolRef Reference = nullptr;
   };
   using BoolRefResource = Elaina::FrameResource<BoolRefDescriptor, BoolRef>;

   using UINT32Ref = UINT32*;
   struct UINT32RefDescriptor
   {
      UINT32Ref Reference = nullptr;
   };
   using UINT32RefResource = Elaina::FrameResource<UINT32RefDescriptor, UINT32Ref>;

   using FloatRef = float*;
   struct FloatRefDescriptor
   {
      FloatRef Reference = nullptr;
   };
   using FloatRefResource = Elaina::FrameResource<FloatRefDescriptor, FloatRef>;

   struct MatrixDescriptor
   {
      Matrix Value;
   };
   using MatrixResource = Elaina::FrameResource<MatrixDescriptor, Matrix>;

   class Mesh;
   using MeshRef = Mesh*;
   struct MeshRefDescriptor
   {
      MeshRef Reference;
   };
   using MeshRefResource = Elaina::FrameResource<MeshRefDescriptor, MeshRef>;

   class DynamicCubemap;
   struct DynamicCubemapDescriptor
   {
      RendererDX11* Renderer = nullptr;
      unsigned int Size = 0;
   };
   using DynamicCubemapResource = Elaina::FrameResource<DynamicCubemapDescriptor, DynamicCubemap>;

   using DynamicCubemapRef = DynamicCubemap*;
   struct DynamicCubemapRefDescriptor
   {
      DynamicCubemapRef Reference = nullptr;
   };
   using DynamicCubemapRefResource = Elaina::FrameResource<DynamicCubemapRefDescriptor, DynamicCubemapRef>;

   class Texture2dDX11;
   using Texture2dDX11Ref = Texture2dDX11*;
   struct Texture2dDX11Descriptor
   {
      RendererDX11* Renderer = nullptr;
      ID3D11Texture2D* Source = nullptr; /** Source가 nullptr면 아래의 정보를 이용하여 realize 합니다 */
      unsigned int Width = 1;
      unsigned int Height = 1;
      unsigned int Channels = 4;
      unsigned char* Data = nullptr;
      DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;
   };
   using Texture2dDX11Resource = Elaina::FrameResource<Texture2dDX11Descriptor, Texture2dDX11>;

   struct Texture2dDX11RefDescriptor
   {
      Texture2dDX11Ref Reference;
   };
   using Texture2dDX11RefResource = Elaina::FrameResource<Texture2dDX11RefDescriptor, Texture2dDX11Ref>;

   using VoidRef = void*;
   struct VoidRefDescriptor
   {
      VoidRef Reference = nullptr;
   };
   using VoidRefResource = Elaina::FrameResource<VoidRefDescriptor, VoidRef>;
}