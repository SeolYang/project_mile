#pragma once
#include <algorithm>
#include <stack>
#include <fstream>
#include <Elaina/FrameResource.h>
#include <Elaina/RenderPass.h>
#include <Elaina/CallbackRenderPass.h>
#include <Elaina/RenderPassBuilder.h>

namespace Elaina
{
   /** Execute 단계에서 실제로 실행될 렌더 패스와 할당 또는 할당 해제될 리소스들의 리스트 */
   struct RenderPhase
   {
      Elaina::RenderPass* RenderPass = nullptr;
      std::vector<FrameResourceBase*> ToRealize;
      std::vector<FrameResourceBase*> ToDerealize;
   };

   /**
   * @param filePath Output path
   * @param bgColor  Background color; "#rgb" | "H S V" | "ColorNmae" (ex. White is "#ffffff" as hexadecimal or "0.0 0.0 1.0" or "White" (https://graphviz.org/doc/info/colors.html)
   * @param fontName Output font name (https://graphviz.org/doc/info/attrs.html#d:fontname)
   * @param fontSize Output font size
   * @param renderPassNodeColor     RenderPass node color
   * @param transientResNodeColor   Trasient Resource node color
   * @param externalResNodeColor    External permanent Resource node color
   * @param createRefEdgeColor      RenderPass->Resource(Create)
   * @param writeRefEdgeColor       RenderPass->Resource(Write)
   * @param readRefEdgeColor        Resource->RenderPass(Read)
   * @graphviz https://sketchviz.com/graphviz-examples
   */
   struct VisualizeParams
   {
      const std::string& FilePath = "output.dot";
      const std::string& FontName = "arial";
      double FontSize = 12.0;
      const std::string& FontColor = "black";
      const std::string& EdgeFontColor = "white";
      double EdgeFontSize = 12.0;
      const std::string& BgColor = "black";
      const std::string& SubgraphBorderColor = "white";
      bool bLeftToRight = true;
      bool bSplines = false;
      double Pad = 0.1;
      double NodeSep = 0.6;
      double RankSep = 1.5;
      const std::string& RenderPassNodeColor = "darkorange";
      const std::string& TransientResNodeColor = "peachpuff";
      const std::string& ExternalResNodeColor = "palegreen";
      const std::string& CreateRefEdgeColor = "aquamarine";
      const std::string& WriteRefEdgeColor = "firebrick1";
      const std::string& ReadRefEdgeColor = "yellow";
   };

   class FrameGraph
   {
   public:
      FrameGraph() = default;
      virtual ~FrameGraph()
      {
         Clear();
      }

      /** Setup */
      template <typename RenderPassType, typename... Args>
      RenderPassType* AddRenderPass(Args&&... args)
      {
         auto* newRenderPass = new RenderPassType(args...);
         RenderPasses.push_back(newRenderPass);

         RenderPassBuilder builder(this, newRenderPass);
         newRenderPass->Setup(builder);

         return newRenderPass;
      }

      template <typename DataType, typename... Args>
      CallbackRenderPass<DataType>* AddCallbackPass(Args&&... args)
      {
         return AddRenderPass<CallbackRenderPass<DataType>>(std::forward<Args>(args)...);
      }

      template <typename DescriptorType, typename ActualType>
      auto AddExternalPermanentResource(const StringType& name, const DescriptorType& descriptor, ActualType* actual)
      {
         auto newResource = new FrameResource<DescriptorType, ActualType>(name, descriptor, actual);
         Resources.push_back(newResource);
         return newResource;
      }

      /** Compile */
      void Compile()
      {
         /** Sort render passes by Distribution Group */
         std::stable_sort(RenderPasses.begin(), RenderPasses.end(),
            [](const RenderPass* lhs, const RenderPass* rhs)
            {
               return lhs->GetDistributionGroup() < rhs->GetDistributionGroup();
            });

         /** Calculate Reference Count */
         for (auto renderPass : RenderPasses)
         {
            renderPass->OnCompile();
         }
         for (auto resource : Resources)
         {
            resource->OnCompile();
         }

         /** Culling resources and render pass */
         std::stack<FrameResourceBase*> expiredResources;
         for (auto resource : Resources)
         {
            if (resource->IsExpired())
            {
               expiredResources.push(resource);
            }
         }

         while (!expiredResources.empty())
         {
            auto expiredResource = expiredResources.top();
            expiredResources.pop();

            auto creator = expiredResource->GetCreator();
            if (!creator->IsNeedToCull())
            {
               --creator->RefCount;
            }

            /** Render pass expired! */
            if (creator->IsNeedToCull())
            {
               /** Cull 처리 될 render pass가 참조하던 리소스의 레퍼런스 카운트를 감소시켜준다 */
               for (auto readResource : creator->Reads)
               {
                  if(readResource->RefCount > 0) /** Do not include external permanent */
                  {
                     --readResource->RefCount;
                  }

                  if (readResource->IsExpired())
                  {
                     expiredResources.push(readResource);
                  }
               }
            }

            for (auto writer : expiredResource->Writers)
            {
               if (!writer->IsNeedToCull())
               {
                  --writer->RefCount;
               }

               if (writer->IsNeedToCull())
               {
                  /** Cull 처리 될 render pass가 참조하던 리소스의 레퍼런스 카운트를 감소시켜준다 */
                  for (auto readResource : creator->Reads)
                  {
                     if (readResource->RefCount > 0) /** Do not include external permanent */
                     {
                        --readResource->RefCount;
                     }

                     if (readResource->IsExpired())
                     {
                        expiredResources.push(readResource);
                     }
                  }
               }
            }
         }

         /** Construct render phases (Find Realized/Derealized resources each render pass) */
         Phases.clear();
         for (auto targetRenderPass : RenderPasses)
         {
            if (!targetRenderPass->IsNeedToCull())
            {
               RenderPhase newPhase;
               newPhase.RenderPass = targetRenderPass;

               /** Always realize create resource in its own render pass. */
               for (auto resource : targetRenderPass->Creates)
               {
                  newPhase.ToRealize.push_back(resource);
                  if (resource->Readers.empty() && resource->Writers.empty())
                  {
                     /** Immediately derealize own resource after render pass excuted if there a no more any referencing by other render pass. */
                     newPhase.ToDerealize.push_back(resource);
                  }
               }

               /** For resources which read and write by current render pass. */
               auto readsAndWrites = targetRenderPass->Reads;
               readsAndWrites.insert(
                  readsAndWrites.end(),
                  targetRenderPass->Writes.begin(), targetRenderPass->Writes.end());

               /** Greedy selection */
               for (auto resource : readsAndWrites)
               {
                  if (resource->IsTransient())
                  {
                     bool bFoundLastReader = false;
                     size_t lastIndex = 0;
                     if (!resource->Readers.empty())
                     {
                        for (auto renderPassItr = RenderPasses.begin(); renderPassItr != RenderPasses.end(); ++renderPassItr)
                        {
                           if ((*renderPassItr) == resource->Readers.back())
                           {
                              bFoundLastReader = true;
                              lastIndex = std::distance(RenderPasses.begin(), renderPassItr);
                           }
                        }
                     }

                     bool bFoundLastWriters = false;
                     if (!resource->Writers.empty())
                     {
                        for (auto renderPassItr = RenderPasses.begin(); renderPassItr != RenderPasses.end(); ++renderPassItr)
                        {
                           if ((*renderPassItr) == resource->Writers.back())
                           {
                              bFoundLastWriters = true;
                              lastIndex = std::max(
                                 lastIndex,
                                 (size_t)std::distance(RenderPasses.begin(), renderPassItr));
                           }
                        }
                     }

                     /** @TODO MORE OPTIMIZE(한번만 어느시점에서 최종적으로 deserialize 될지는 한번만 계산해도됨 */
                     bool bNeedToDerealizeOnCurrent = targetRenderPass == RenderPasses[lastIndex];
                     if ((bFoundLastWriters || bFoundLastReader) && bNeedToDerealizeOnCurrent)
                     {
                        newPhase.ToDerealize.push_back(resource);
                     }
                  }
               }

               Phases.push_back(std::move(newPhase));
            }
         }
      }

      /** Execute */
      void Execute()
      {
         for (auto phase : Phases)
         {
            /* Realize resource */
            for (auto resource : phase.ToRealize)
            {
               resource->Realize();
            }

            phase.RenderPass->Execute();

            /* Derealize Resource*/
            for (auto resource : phase.ToDerealize)
            {
               resource->Derealize();
            }
         }
      }

      /** If a distribution group executed, then it makes able to execute only with equal or exceed distribution group! */
      void ExecuteDistributionGroup(const size_t distributionGroup)
      {
         if (LatestExecutedDistributionGruop <= distributionGroup)
         {
            for (size_t idx = LatestExcutedIndex; idx < Phases.size(); ++idx)
            {
               auto phase = Phases[idx];
               if (phase.RenderPass->GetDistributionGroup() == distributionGroup)
               {
                  /* Realize resource */
                  for (auto resource : phase.ToRealize)
                  {
                     resource->Realize();
                  }

                  phase.RenderPass->Execute();

                  /* Derealize Resource*/
                  for (auto resource : phase.ToDerealize)
                  {
                     resource->Derealize();
                  }
               }
               else if (phase.RenderPass->GetDistributionGroup() > distributionGroup)
               {
                  LatestExcutedIndex = idx;
                  break;
               }
            }

            LatestExecutedDistributionGruop = distributionGroup;
         }
      }

      void Clear()
      {
         for (auto resource : Resources)
         {
            SafeDelete(resource);
         }

         for (auto renderPass : RenderPasses)
         {
            SafeDelete(renderPass);
         }

         RenderPasses.clear();
         Resources.clear();
         Phases.clear();

         LatestExecutedDistributionGruop = 0;
         LatestExcutedIndex = 0;
      }


      void ExportVisualization(const VisualizeParams& params)
      {
         std::ofstream stream(params.FilePath);

         stream << "digraph FrameGraph \n{\n";

         stream << "splines=" << (params.bSplines ? "true" : "false") << std::endl;

         stream << "graph [pad=\"" << params.Pad 
            << "\", nodestep=\"" << params.NodeSep
            << "\", ranksep=\"" << params.RankSep << "\"]" << std::endl;

         stream << "rankdir = " << (params.bLeftToRight ? "LR" : "TB") << std::endl;;
         stream << "bgcolor = " << params.BgColor << std::endl;
         stream << "node [shape=rectangle, fontname=\""
            << params.FontName
            << "\", fontsize=" << params.FontSize
            << ", fontcolor=" << params.FontColor << "]" << std::endl;

         stream << "edge [fontname=\"" << params.FontName << "\""
            << ", fontsize=" << params.EdgeFontSize
            << ", fontcolor=" << params.EdgeFontColor << "]" << std::endl;

         /** Export Render Passes as graph nodes */
         int latestDistributionGroup = -1;
         for (auto renderPass : RenderPasses)
         {
            if (latestDistributionGroup < (int)renderPass->GetDistributionGroup())
            {
               if (latestDistributionGroup != -1)
               {
                  stream << "}" << std::endl;
               }

               latestDistributionGroup = renderPass->GetDistributionGroup();
               stream << "subgraph cluster_distribution_group" << latestDistributionGroup << std::endl;
               stream << "{" << std::endl << "label=\"Distribution Group " << latestDistributionGroup << "\";" << std::endl;
               stream << "fontname=\"" << params.FontName << "\";" << std::endl;
               stream << "fontcolor=" << params.EdgeFontColor << ";" << std::endl;
               stream << "color=" << params.SubgraphBorderColor << ";" << std::endl;
            }

            stream << "\"" << renderPass->GetName() <<
               "\" [label=\"" << renderPass->GetName() << std::endl
               << "\\nDistribution Group : " << renderPass->GetDistributionGroup() << std::endl
               << "\\nRefs : " << renderPass->GetRefCount() <<
               "\", style=filled, fillcolor=" << params.RenderPassNodeColor << "];" << std::endl;
         }
         stream << "}" << std::endl;

         /** Export Resources as graph nodes */
         for (auto resource : Resources)
         {
            stream << "\"" << resource->GetName() <<
               "\" [label=\"" << resource->GetName() << std::endl
               << "\\nID : " << resource->GetID() << std::endl
               << "\\nRefs : " << resource->GetRefCount() << std::endl
               << "\\n" << (resource->IsTransient() ? "Transient" : "External Permanent")
               << "\", style=filled, fillcolor=" << (resource->IsTransient() ? params.TransientResNodeColor : params.ExternalResNodeColor) << "]" << std::endl;
         }

         /** Export resource referencing relation as directed graph edge */
         /** RenderPass->Resource Referencing relations */
         for (auto renderPass : RenderPasses)
         {
            /** Create Resource */
            stream << "\"" << renderPass->GetName() << "\" -> { ";
            for (auto resource : renderPass->Creates)
            {
               stream << "\"" << resource->GetName() << "\" ";
            }
            stream << "} [color=" << params.CreateRefEdgeColor
               << "]"
               << std::endl;

            /** Write Resource */
            stream << "\"" << renderPass->GetName() << "\" -> { ";
            for (auto resource : renderPass->Writes)
            {
               stream << "\"" << resource->GetName() << "\" ";
            }
            stream << "} [color=" << params.WriteRefEdgeColor
               << "]"
               << std::endl;
         }
         stream << std::endl;

         /** Resource->RenderPass Referencing relations */
         for (auto resource : Resources)
         {
            stream << "\"" << resource->GetName() << "\" -> { ";
            for (auto renderPass : resource->Readers)
            {
               stream << "\"" << renderPass->GetName() << "\" ";
            }
            stream << "} [color=" << params.ReadRefEdgeColor
               << "]"
               << std::endl;
         }

         stream << "}"; // End of diagraph FrameGraph
      }

   private:
      std::vector<RenderPass*> RenderPasses;
      std::vector<FrameResourceBase*> Resources;
      std::vector<RenderPhase> Phases;

      size_t LatestExecutedDistributionGruop = 0;
      size_t LatestExcutedIndex = 0;

      friend RenderPassBuilder;

   };

   template <typename ResourceType, typename DescriptorType>
   ResourceType* RenderPassBuilder::Create(const StringType& name, const DescriptorType& descriptor)
   {
      ResourceType* newResource = new ResourceType(name, TargetRenderPass, descriptor);
      TargetFrameGraph->Resources.push_back(newResource);
      TargetRenderPass->Creates.push_back(newResource);
      return newResource;
   }
}