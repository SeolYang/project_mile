#pragma once
#include <Elaina/Elaina.config.h>
#include <Elaina/RenderPass.h>

namespace Elaina
{
   class FrameGraph;
   class FrameResourceBase;
   class RenderPass;
   class RenderPassBuilder
   {
   public:
      RenderPassBuilder(FrameGraph* targetFrameGraph, RenderPass* targetRenderPass) :
         TargetFrameGraph(targetFrameGraph),
         TargetRenderPass(targetRenderPass)
      {
      }

      ~RenderPassBuilder() = default;

      template <typename ResourceType, typename DescriptorType>
      ResourceType* Create(const StringType& name, const DescriptorType& descriptor);

      template <typename ResourceType>
      ResourceType* Read(ResourceType* resource)
      {
         RenderPass* resourceCreator = resource->GetCreator();
         if (resource->IsExternalPermanent() ||
            TargetRenderPass->GetDistributionGroup() >= resourceCreator->GetDistributionGroup())
         {
            resource->Readers.push_back(TargetRenderPass);
            TargetRenderPass->Reads.push_back(resource);
            return resource;
         }

         std::string errorMessage = "Resource(";
         errorMessage.append(resource->GetName());
         errorMessage.append(") creator's distribution group exceed ");
         errorMessage.append(TargetRenderPass->GetName());
         errorMessage.append("'s distribution group.");
         ELAINA_ASSERT(false, errorMessage.c_str());
         return nullptr;
      }

      template <typename ResourceType>
      ResourceType* Write(ResourceType* resource)
      {
         RenderPass* resourceCreator = resource->GetCreator();
         if (resource->IsExternalPermanent() ||
            TargetRenderPass->GetDistributionGroup() >= resourceCreator->GetDistributionGroup())
         {
            resource->Writers.push_back(TargetRenderPass);
            TargetRenderPass->Writes.push_back(resource);
            return resource;
         }

         std::string errorMessage = "Resource(";
         errorMessage.append(resource->GetName());
         errorMessage.append(") creator's distribution group exceed ");
         errorMessage.append(TargetRenderPass->GetName());
         errorMessage.append("'s distribution group.");
         ELAINA_ASSERT(false, errorMessage.c_str());
         return nullptr;
      }

   private:
      FrameGraph* TargetFrameGraph;
      RenderPass* TargetRenderPass;

   };
}
