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

      bool Vector3Input(const std::string& name, Vector3& target, float speed, float _min, float _max, bool enableLog)
      {
         return ImGui::DragFloat3(name.c_str(), &target.x, speed, _min, _max, "%.03f", enableLog ? ImGuiSliderFlags_Logarithmic : 0);
      }

      bool Vector4Input(const std::string& name, Vector4& target, float speed, float _min, float _max, bool enableLog)
      {
          return ImGui::DragFloat4(name.c_str(), &target.x, speed, _min, _max, "%.03f", enableLog ? ImGuiSliderFlags_Logarithmic : 0);
      }

      bool FloatInput(const std::string& name, float& target, float speed, float _min, float _max, bool enableLog)
      {
         return ImGui::DragFloat(name.c_str(), &target, 0.1f, _min, _max, "%.03f", enableLog ? ImGuiSliderFlags_Logarithmic : 0);
      }
   }
}