#include "Rendering/RendererPBR.h"
#include "GameFramework/World.h"
#include "Component/CameraComponent.h"
#include "Component/LightComponent.h"
#include "Component/MeshRenderComponent.h"

namespace Elaina
{
   using namespace Mile;
   template<>
   std::vector<CameraComponent*>* Realize(const WorldDescriptor& descriptor)
   {
      std::vector<CameraComponent*>* cameras = new std::vector<CameraComponent*>();
      if (descriptor.TargetWorld != nullptr)
      {
         (*cameras) = std::move(descriptor.TargetWorld->GetComponentsFromEntities<CameraComponent>());
      }

      return cameras;
   }

   template<>
   std::vector<LightComponent*>* Realize(const WorldDescriptor& descriptor)
   {
      std::vector<LightComponent*>* lights = new std::vector<LightComponent*>();
      if (descriptor.TargetWorld != nullptr)
      {
         (*lights) = std::move(descriptor.TargetWorld->GetComponentsFromEntities<LightComponent>());
      }

      return lights;
   }

   template<>
   std::vector<MeshRenderComponent*>* Realize(const WorldDescriptor& descriptor)
   {
      std::vector<MeshRenderComponent*>* meshes = new std::vector<MeshRenderComponent*>();
      if (descriptor.TargetWorld != nullptr)
      {
         (*meshes) = std::move(descriptor.TargetWorld->GetComponentsFromEntities<MeshRenderComponent>());
      }

      return meshes;
   }
}

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileRendererPBR);

   RendererPBR::RendererPBR(Context* context, size_t maximumThreads) :
      RendererDX11(context, maximumThreads)
   {
   }

   RendererPBR::~RendererPBR()
   {
      m_frameGraph.Clear();
   }

   bool RendererPBR::Init(Window& window)
   {
      if (RendererDX11::Init(window))
      {
         if (ConstructFrameGraph())
         {
            return true;
         }
      }

      ME_LOG(MileRendererPBR, Fatal, TEXT("Failed to initialize PBR Renderer!"));
      return false;
   }

   void RendererPBR::RenderImpl(const World& world)
   {
   }

   bool RendererPBR::ConstructFrameGraph()
   {
      auto cameraInfoRes = m_frameGraph.AddExternalPermanentResource("Cameras", WorldDescriptor(), &m_cameras);
      auto lightInfoRes = m_frameGraph.AddExternalPermanentResource("Lights", WorldDescriptor(), &m_lights);
      auto meshInfoRes = m_frameGraph.AddExternalPermanentResource("Meshes", WorldDescriptor(), &m_meshes);

      return true;
   }

}