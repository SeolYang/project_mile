#include "Layers/WorldHierarchyLayer.h"
#include "GameFramework/World.h"
#include "GameFramework/Entity.h"
#include "GameFramework/Transform.h"
#include "Core/imguiHelper.h"

namespace Mile
{
   namespace Editor
   {
      DEFINE_LOG_CATEGORY(MileWorldHierarchyLayer);

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
         if (targetRoot != nullptr && targetRoot->IsVisibleOnHierarchy())
         {
            const std::string& entityName = targetRoot->GetNameUTF8();

            ImGuiTreeNodeFlags flags = 
               ((m_selectedEntity == targetRoot) ? ImGuiTreeNodeFlags_Selected : 0)
               | ImGuiTreeNodeFlags_OpenOnArrow 
               | ImGuiTreeNodeFlags_SpanAvailWidth;
            bool bIsOpened = ImGui::TreeNodeEx((void*)targetRoot, flags, entityName.c_str());
            if (ImGui::IsItemClicked())
            {
               m_selectedEntity = targetRoot;
               m_tempEulerRotation = Math::QuaternionToEulerAngles(m_selectedEntity->GetTransform()->GetRotation());
               ME_LOG(MileWorldHierarchyLayer, Log, TEXT("Entity has been selected at hierarchy : ") + m_selectedEntity->GetName());
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
         ImGui::Begin("Hierarchy");
         std::string worldName = WString2String(m_target->GetWorldName());
         if (ImGui::TreeNode(worldName.c_str()))
         {
            if (m_target != nullptr)
            {
               const auto& rootEntities = m_target->GetRootEntities();
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
         ImGui::Begin("Properties");
         if (m_selectedEntity != nullptr)
         {
            ImGui::Spacing();
            if (m_target->CheckEntityValidation(m_selectedEntity))
            {
               bool bIsEntityActivated = m_selectedEntity->IsActivated();
               if (ImGui::Checkbox("##entityActivationCheckbox0", &bIsEntityActivated))
               {
                  m_selectedEntity->SetActive(bIsEntityActivated);
               }

               ImGui::SameLine();

               char buffer[DEFAULT_STR_INPUT_BUFFER_SIZE] = { 0 };
               strcpy_s(buffer, WString2String(m_selectedEntity->GetName()).c_str());
               if (ImGui::InputText("##entityNameInput0", buffer, DEFAULT_STR_INPUT_BUFFER_SIZE))
               {
                  std::string newName = buffer;
                  m_selectedEntity->SetName(String2WString(newName));
               }

               ImGui::Spacing();
               ImGui::Separator();
               ImGui::Spacing();

               if (ImGui::CollapsingHeader("Transform"))
               {
                  Transform* entitiyTransform = m_selectedEntity->GetTransform();
                  auto position = entitiyTransform->GetPosition();
                  if (GUI::Vector3Input("Position", position))
                  {
                     entitiyTransform->SetPosition(position);
                  }

                  if (GUI::Vector3Input("Rotation", m_tempEulerRotation))
                  {
                     entitiyTransform->SetRotation(Math::EulerToQuaternion(m_tempEulerRotation));
                  }

                  auto scale = entitiyTransform->GetScale();
                  if (GUI::Vector3Input("Scale", scale))
                  {
                     entitiyTransform->SetScale(scale);
                  }

                  ImGui::Spacing();
                  ImGui::Separator();
                  ImGui::Spacing();
               }

               auto& components = m_selectedEntity->GetComponents();
               for (auto component : components)
               {
                  String typeStr = component->GetType();
                  if (ImGui::CollapsingHeader(WString2String(typeStr).c_str()))
                  {
                     component->OnGUI();

                     ImGui::Spacing();
                     ImGui::Separator();
                     ImGui::Spacing();
                  }
               }
            }
            else
            {
               m_selectedEntity = nullptr;
            }
         }
         ImGui::End();
      }
   }
}