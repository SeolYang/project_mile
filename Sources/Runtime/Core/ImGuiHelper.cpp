#include "Core/ImGuiHelper.h"

namespace Mile
{
   namespace GUI
   {
      MEAPI void ImageRelativeToWindow(ID3D11ShaderResourceView* srv, const Vector2& resolution)
      {
         auto contentRegion = ImGui::GetContentRegionAvail();

         ImVec2 windowSize = ImGui::GetWindowSize();
         float titleBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight();
         float menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();
         float padding = menuBarHeight + titleBarHeight;
         ImVec2 actualContentArea = ImVec2{ windowSize.x, windowSize.y - padding };

         Vector2 relativeOutputRes = FindResolutionWithAspectRatio(actualContentArea.x, actualContentArea.y, resolution.x / resolution.y);
         if (resolution.x < actualContentArea.x && resolution.y < actualContentArea.y)
         {
            relativeOutputRes = resolution;
         }

         ImVec2 outputRes{ relativeOutputRes.x, relativeOutputRes.y };
         ImGui::SetCursorPosX((actualContentArea.x - outputRes.x) * 0.5f);
         ImGui::SetCursorPosY((actualContentArea.y - outputRes.y) * 0.5f + padding);
         ImGui::Image((void*)srv, outputRes);
      }

      bool Vector3Input(const std::string& name, Vector3& target, float speed, float _min, float _max, bool enableLog, const char* format)
      {
         std::string prefix = "##";

         ImGui::TableNextRow();
         ImGui::TableSetColumnIndex(0);
         ImGui::Text(name.c_str());
         ImGui::TableSetColumnIndex(1);
         return ImGui::DragFloat3((prefix.append(name)).c_str(), &target.x, speed, _min, _max, format, enableLog ? ImGuiSliderFlags_Logarithmic : 0);
      }

      bool Vector4Input(const std::string& name, Vector4& target, float speed, float _min, float _max, bool enableLog, const char* format)
      {
         std::string prefix = "##";

         ImGui::TableNextRow();
         ImGui::TableSetColumnIndex(0);
         ImGui::Text(name.c_str());
         ImGui::TableSetColumnIndex(1);
          return ImGui::DragFloat4((prefix.append(name)).c_str(), &target.x, speed, _min, _max, format, enableLog ? ImGuiSliderFlags_Logarithmic : 0);
      }

      bool FloatInput(const std::string& name, float& target, float speed, float _min, float _max, bool enableLog, const char* format)
      {
         std::string prefix = "##";

         ImGui::TableNextRow();
         ImGui::TableSetColumnIndex(0);
         ImGui::Text(name.c_str());
         ImGui::TableSetColumnIndex(1);
         return ImGui::DragFloat((prefix.append(name)).c_str(), &target, 0.1f, _min, _max, format, enableLog ? ImGuiSliderFlags_Logarithmic : 0);
      }

      bool Checkbox(const std::string& name, bool& value)
      {
         std::string prefix = "##";
         ImGui::TableNextRow();
         ImGui::TableSetColumnIndex(0);
         ImGui::Text(name.c_str());
         ImGui::TableSetColumnIndex(1);
         return ImGui::Checkbox((prefix.append(name)).c_str(), &value);
      }
      
      bool Combobox(const std::string& name, const std::vector<std::string>& items, const std::string& currentItem, unsigned int& enumValue)
      {
         std::string prefix = "##";
         ImGui::TableNextRow();
         ImGui::TableSetColumnIndex(0);
         ImGui::Text(name.c_str());
         ImGui::TableSetColumnIndex(1);
         if (ImGui::BeginCombo(prefix.append(name).c_str(), currentItem.c_str()))
         {
            for (UINT32 idx = 0; idx < items.size(); ++idx)
            {
               bool bSelected = (currentItem == items[idx]);
               if (ImGui::Selectable(items[idx].c_str(), bSelected))
               {
                  enumValue = idx;
               }
               if (bSelected)
               {
                  ImGui::SetItemDefaultFocus();
               }
            }

            ImGui::EndCombo();
            return true;
         }

         return false;
      }

      MEAPI bool TreeNode(const std::string& name, bool bOpened)
      {
         ImGui::TableNextRow();
         ImGui::TableSetColumnIndex(0);
         bool ret = ImGui::TreeNodeEx(name.c_str(), bOpened ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None);
         return ret;
      }
   }
}