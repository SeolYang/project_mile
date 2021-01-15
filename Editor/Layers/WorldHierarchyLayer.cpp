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
         m_selectedEntity(nullptr),
         Layer(context, TEXT("WorldHierarchyLayer"))
      {
      }

      void WorldHierarchyLayer::OnIMGUIRender()
      {
         DrawHierarchyPanel();
         DrawPropertiesPanel();
      }

      void WorldHierarchyLayer::DrawEntityNode(Entity* targetRoot)
      {
         if (targetRoot != nullptr)
         {
            std::string entityName = WString2String(targetRoot->GetName());

            ImGuiTreeNodeFlags flags = 
               ((m_selectedEntity == targetRoot) ? ImGuiTreeNodeFlags_Selected : 0)
               | ImGuiTreeNodeFlags_OpenOnArrow 
               | ImGuiTreeNodeFlags_SpanAvailWidth;
            bool bIsOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)targetRoot, flags, entityName.c_str());
            if (ImGui::IsItemClicked())
            {
               m_selectedEntity = targetRoot;
            }

            bool bIsEntityDeleted = false;
            if (ImGui::BeginPopupContextItem())
            {
               if (ImGui::MenuItem("Delete Entity"))
               {
                  bIsEntityDeleted = true;
               }

               ImGui::EndPopup();
            }

            if (bIsOpened)
            {
               auto children = targetRoot->GetChildren();
               for (Entity* child : children)
               {
                  DrawEntityNode(child);
               }

               ImGui::TreePop();
            }

            if (bIsEntityDeleted)
            {
               m_target->DestroyEntity(targetRoot);
            }
         }
      }

      void WorldHierarchyLayer::DrawHierarchyPanel()
      {
         ImGui::Begin("World Hierarchy");
         if (ImGui::TreeNode("Scene"))
         {
            if (m_target != nullptr)
            {
               auto rootEntities = m_target->GetRootEntities();
               for (Entity* rootEntity : rootEntities)
               {
                  DrawEntityNode(rootEntity);
               }
            }

            ImGui::TreePop();
         }
         ImGui::End();
      }

      void WorldHierarchyLayer::DrawPropertiesPanel()
      {
         if (m_selectedEntity != nullptr)
         {

         }
      }
   }
}