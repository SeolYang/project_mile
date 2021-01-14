#include "Layers/SceneHierarchyLayer.h"
#include "GameFramework/World.h"
#include "GameFramework/Entity.h"
#include "imgui.h"

namespace Mile
{
   namespace Editor
   {
      SceneHierarchyLayer::SceneHierarchyLayer(Context* context) :
         m_target(nullptr),
         Layer(context, TEXT("SceneHierarchyLayer"))
      {
      }

      void SceneHierarchyLayer::OnIMGUIRender()
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

      void SceneHierarchyLayer::ConstructTree(Entity* targetRoot)
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