#pragma once
#include "Rendering/RendererDX11.h"
#include "Elaina/FrameGraph.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileRendererPBR, Log);

   struct WorldDescriptor
   {
      World* TargetWorld = nullptr;
   };

   class CameraComponent;
   struct CameraDescriptor
   {
      CameraComponent* TargetCamera = nullptr;
   };

   class LightComponent;
   class MeshRenderComponent;
   using Lights = std::vector<LightComponent*>;
   using Meshes = std::vector<MeshRenderComponent*>;

   using CameraInfoResource = Elaina::FrameResource<CameraDescriptor, CameraComponent*>;
   using LightsInfoResource = Elaina::FrameResource<WorldDescriptor, Lights>;
   using MeshesInfoResource = Elaina::FrameResource<WorldDescriptor, Meshes>;

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

   class MEAPI RendererPBR : public RendererDX11
   {
   public:
      RendererPBR(Context* context, size_t maximumThreads);
      virtual ~RendererPBR();

      bool Init(Window& window) override;

   protected:
      void RenderImpl(const World& world) override;

   private:
      bool ConstructFrameGraph();

   private:
      Elaina::FrameGraph m_frameGraph;
      std::vector<CameraComponent*> m_cameras;
      CameraComponent* m_targetCamera;
      Lights m_lights;
      Meshes m_meshes;

   };
}