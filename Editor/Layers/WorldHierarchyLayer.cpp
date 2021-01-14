#include "Layers/WorldHierarchyLayer.h"
#include "GameFramework/World.h"
#include "GameFramework/Entity.h"
#include "imgui.h"

namespace Mile
{
   namespace Editor
   {
      WorldHierarchyLayer::WorldHierarchyLayer(Context* context) :
         m_target(nullptr),
         Layer(context, TEXT("WorldHierarchyLayer"))
      {
      }

      void WorldHierarchyLayer::OnIMGUIRender()
      {
         ImGui::Begin("Scene Hierarchy");
         if (ImGui::TreeNode("Scene"))
         {
            if (m_target != nullptr)
            {
               auto rootEntities = m_target->GetRootEntities();
               for (Entity* rootEntity : rootEntities)
               {
                  ConstructTree(rootEntity);
               }
            }

            ImGui::TreePop();
         }
         ImGui::End();
      }

      void WorldHierarchyLayer::ConstructTree(Entity* targetRoot)
      {
         if (targetRoot != nullptr)
         {
            std::string entityName = WString2String(targetRoot->GetName());
            if (ImGui::TreeNode(entityName.c_str()))
            {
               auto children = targetRoot->GetChildren();
               for (Entity* child : children)
               {
                  ConstructTree(child);
               }

               ImGui::TreePop();
            }
         }
      }
   }
}