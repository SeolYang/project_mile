#include "Layers/WorldHierarchyLayer.h"
#include "GameFramework/World.h"
#include "GameFramework/Entity.h"
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
            std::string entityName = WString2String(targetRoot->GetName());

            ImGuiTreeNodeFlags flags = 
               ((m_selectedEntity == targetRoot) ? ImGuiTreeNodeFlags_Selected : 0)
               | ImGuiTreeNodeFlags_OpenOnArrow 
               | ImGuiTreeNodeFlags_SpanAvailWidth;
            bool bIsOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)targetRoot, flags, entityName.c_str());
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
         ImGui::Begin("World Hierarchy");
         WindowFocusedEffect(0, 255, 0);
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
         ImGui::Begin("Properties");
         WindowFocusedEffect(0, 255, 0);
         if (m_selectedEntity != nullptr)
         {
            if (m_target->CheckEntityValidation(m_selectedEntity))
            {
               ImGui::Text("Entity Properties");
               bool bIsEntityActivated = m_selectedEntity->IsActivated();
               if (ImGui::Checkbox("Activate", &bIsEntityActivated))
               {
                  m_selectedEntity->SetActive(bIsEntityActivated);
               }

               char buffer[DEFAULT_STR_INPUT_BUFFER_SIZE] = { 0 };
               strcpy_s(buffer, WString2String(m_selectedEntity->GetName()).c_str());
               if (ImGui::InputText("Name", buffer, DEFAULT_STR_INPUT_BUFFER_SIZE))
               {
                  std::string newName = buffer;
                  m_selectedEntity->SetName(String2WString(newName));
               }

               ImGui::Dummy(ImVec2(0.0f, 5.0f));
               ImGui::Text("Transform");
               Transform* entitiyTransform = m_selectedEntity->GetTransform();
               auto position = entitiyTransform->GetPosition();
               if (ImGui::InputFloat3("Position(X, Y, Z)", position.elements, "%.3f"))
               {
                  entitiyTransform->SetPosition(position);
               }

               if (ImGui::InputFloat3("Rotation(Pitch, Yaw, Roll)", m_tempEulerRotation.elements, "%.3f"))
               {
                  entitiyTransform->SetRotation(Math::EulerToQuaternion(m_tempEulerRotation));
               }

               auto scale = entitiyTransform->GetScale();
               if (ImGui::InputFloat3("Scale(X, Y, Z)", scale.elements, "%.3f"))
               {
                  entitiyTransform->SetScale(scale);
               }

               ImGui::Dummy(ImVec2(0.0f, 5.0f));
               ImGui::Text("Components");
               ImGui::Dummy(ImVec2(0.0f, 5.0f));
               auto& components = m_selectedEntity->GetComponents();
               for (auto component : components)
               {
                  component->OnGUIBegin();
                  component->OnGUI();
                  component->OnGUIEnd();
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