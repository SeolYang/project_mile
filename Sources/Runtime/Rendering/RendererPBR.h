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
   using CameraInfoResource = Elaina::FrameResource<WorldDescriptor, std::vector<CameraComponent*>>;
   class LightComponent;
   using LightInfoResource = Elaina::FrameResource<WorldDescriptor, std::vector<LightComponent*>>;
   class MeshRenderComponent;
   using MeshInfoResource = Elaina::FrameResource<WorldDescriptor, std::vector<MeshRenderComponent*>>;

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
      std::vector<LightComponent*> m_lights;
      std::vector<MeshRenderComponent*> m_meshes;

   };
}