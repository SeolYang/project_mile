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
         m_transformSpace(ETransformSpace::Local),
         Layer(context, TEXT("WorldHierarchyLayer"))
      {
      }

      void WorldHierarchyLayer::OnIMGUIRender()
      {
         OPTICK_EVENT();
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
               m_tempPosition = m_selectedEntity->GetTransform()->GetPosition(m_transformSpace);
               m_tempEulerRotation = Math::QuaternionToEulerAngles(m_selectedEntity->GetTransform()->GetRotation(m_transformSpace));
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
         OPTICK_EVENT();
         ImGui::Begin("Hierarchy");
         std::string worldName = WString2String(m_target->GetWorldName());
         if (ImGui::TreeNodeEx(worldName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
         OPTICK_EVENT();
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

               if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
               {
                  ImGui::Columns(2, "ss");
                  {
                     // Property Names
                     {
                        ImGui::Spacing();
                        ImGui::Text("Transform Space");
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Text("Position");
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Text("Rotation");
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Text("Scale");
                     }

                     ImGui::NextColumn();

                     // Properties
                     {
                        const char* items[] = { "World", "Local" };
                        static const char* currentItem = items[static_cast<UINT32>(m_transformSpace)];

                        ImGui::Spacing();
                        if (ImGui::BeginCombo("##Space", currentItem))
                        {
                           for (UINT32 idx = 0; idx < IM_ARRAYSIZE(items); ++idx)
                           {
                              bool bSelected = (currentItem == items[idx]);
                              if (ImGui::Selectable(items[idx], bSelected))
                              {
                                 currentItem = items[idx];
                                 m_transformSpace = static_cast<ETransformSpace>(idx);
                                 m_tempEulerRotation = Math::QuaternionToEulerAngles(m_selectedEntity->GetTransform()->GetRotation(m_transformSpace));
                                 m_tempPosition = m_selectedEntity->GetTransform()->GetPosition(m_transformSpace);
                              }
                              if (bSelected)
                              {
                                 ImGui::SetItemDefaultFocus();
                              }
                           }

                           ImGui::EndCombo();
                        }

                        Transform* entitiyTransform = m_selectedEntity->GetTransform();
                        auto position = entitiyTransform->GetPosition(m_transformSpace);
                        if (GUI::Vector3Input("##Position", position))
                        {
                           entitiyTransform->SetPosition(position, m_transformSpace);
                        }

                        if (GUI::Vector3Input("##Rotation", m_tempEulerRotation))
                        {
                           entitiyTransform->SetRotation(Math::EulerToQuaternionInOrder(m_tempEulerRotation), m_transformSpace);
                        }

                        auto scale = entitiyTransform->GetScale(m_transformSpace);
                        if (GUI::Vector3Input("##Scale", scale))
                        {
                           entitiyTransform->SetScale(scale, m_transformSpace);
                        }
                     }
                  }
                  ImGui::EndColumns();

                  ImGui::Spacing();
                  ImGui::Separator();
                  ImGui::Spacing();
               }

               auto& components = m_selectedEntity->GetComponents();
               for (auto component : components)
               {
                  std::string typeStr = WString2String(component->GetType());
                  if (ImGui::CollapsingHeader(typeStr.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                  {
                     ImGui::Columns(2, (typeStr.append("_col")).c_str());
                     component->OnGUI();
                     ImGui::EndColumns();

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