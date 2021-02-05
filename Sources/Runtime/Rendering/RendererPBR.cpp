#include "Rendering/RendererPBR.h"
#include "Rendering/RenderTargetDX11.h"
#include "GameFramework/World.h"
#include "Component/CameraComponent.h"
#include "Component/LightComponent.h"
#include "Component/MeshRenderComponent.h"

namespace Elaina
{
   using namespace Mile;
   template<>
   CameraComponent** Realize(const CameraDescriptor& descriptor)
   {
      return new CameraComponent*(descriptor.TargetCamera);
   }

   template<>
   Lights* Realize(const WorldDescriptor& descriptor)
   {
      std::vector<LightComponent*>* lights = new std::vector<LightComponent*>();
      if (descriptor.TargetWorld != nullptr)
      {
         (*lights) = std::move(descriptor.TargetWorld->GetComponentsFromEntities<LightComponent>());
      }

      return lights;
   }

   template<>
   Meshes* Realize(const WorldDescriptor& descriptor)
   {
      std::vector<MeshRenderComponent*>* meshes = new std::vector<MeshRenderComponent*>();
      if (descriptor.TargetWorld != nullptr)
      {
         (*meshes) = std::move(descriptor.TargetWorld->GetComponentsFromEntities<MeshRenderComponent>());
      }

      return meshes;
   }

   template<>
   RenderTargetDX11* Realize(const RenderTargetDescriptor& descriptor)
   {
      auto renderTarget = new RenderTargetDX11(descriptor.Renderer);
      bool bInitialized = false;
      if (descriptor.RenderTargetView != nullptr)
      {
         bInitialized = renderTarget->Init(descriptor.RenderTargetView, descriptor.DepthStencilBuffer);
      }
      else
      {
         bInitialized = renderTarget->Init(descriptor.Width, descriptor.Height, descriptor.Format, descriptor.DepthStencilBuffer);
      }

      if (!bInitialized)
      {
         Mile::SafeDelete(renderTarget);
      }

      return renderTarget;
   }
}

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileRendererPBR);

   RendererPBR::RendererPBR(Context* context, size_t maximumThreads) :
      RendererDX11(context, maximumThreads),
      m_targetCamera(nullptr)
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
      auto cameraInfoRes = m_frameGraph.AddExternalPermanentResource("Camera", CameraDescriptor(), &m_targetCamera);
      auto lightsInfoRes = m_frameGraph.AddExternalPermanentResource("Lights", WorldDescriptor(), &m_lights);
      auto meshesInfoRes = m_frameGraph.AddExternalPermanentResource("Meshes", WorldDescriptor(), &m_meshes);

      return true;
   }

}