#pragma once
#include "Rendering/RendererDX11.h"
#include "Elaina/FrameGraph.h"
#include <map>

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileRendererPBR, Log);

   class CameraComponent;
   using CameraRef = CameraComponent*;
   struct CameraRefDescriptor
   {
      CameraRef Reference;
   };
   using CameraRefResource = Elaina::FrameResource<CameraRefDescriptor, CameraRef>;

   struct WorldDescriptor
   {
      World* TargetWorld = nullptr;
   };

   class LightComponent;
   using Lights = std::vector<LightComponent*>;
   using LightsInfoResource = Elaina::FrameResource<WorldDescriptor, Lights>;

   class MeshRenderComponent;
   using Meshes = std::vector<MeshRenderComponent*>;
   using MeshesInfoResource = Elaina::FrameResource<WorldDescriptor, Meshes>;

   class Material;
   using MaterialMap = std::map<Material*, std::vector<MeshRenderComponent*>>;
   using MaterialMapResource = Elaina::FrameResource<WorldDescriptor, MaterialMap>;

   class RenderTargetDX11;
   struct RenderTargetDescriptor
   {
      RendererDX11* Renderer = nullptr;
      unsigned int Width = 1920;
      unsigned int Height = 1080;
      EColorFormat Format = EColorFormat::R8G8B8A8_UNORM;
      ID3D11RenderTargetView* RenderTargetView = nullptr;
      DepthStencilBufferDX11* DepthStencilBuffer = nullptr;
   };
   using RenderTargetResource = Elaina::FrameResource<RenderTargetDescriptor, RenderTargetDX11>;

   using RenderTargetRef = RenderTargetDX11*;
   struct RenderTargetRefDescriptor
   {
      RenderTargetRef Reference;
   };
   using RenderTargetRefResource = Elaina::FrameResource<RenderTargetRefDescriptor, RenderTargetRef>;

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

   class MEAPI RendererPBR : public RendererDX11
   {
   public:
      RendererPBR(Context* context, size_t maximumThreads);
      virtual ~RendererPBR();

      bool Init(Window& window) override;

   protected:
      void RenderImpl(const World& world) override;

   private:
      bool InitShader();
      bool InitFrameGraph();

   private:
      Elaina::FrameGraph m_frameGraph;

      /** Per Frame Datas */
      std::vector<CameraComponent*> m_cameras;
      CameraComponent* m_targetCamera;
      Lights m_lights;
      Meshes m_meshes;
      MaterialMap m_materialMap;
      RenderTargetDX11* m_outputRenderTarget;

      /** Shaders */
      VertexShaderDX11* m_geometryPassVS;
      PixelShaderDX11* m_geometryPassPS;

   };
}