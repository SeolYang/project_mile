#include "Core/ImGuiHelper.h"
#include "Math/MathMinimal.h"
#include "GameFramework/Transform.h"

namespace Mile
{
   namespace GUI
   {
      bool Vector3Input(const std::string& name, Vector3& target, float speed, float _min, float _max, bool enableLog)
      {
         return ImGui::DragFloat3(name.c_str(), &target.x, speed, _min, _max, "%.03f", enableLog ? ImGuiSliderFlags_Logarithmic : 0);
      }

      MEAPI bool Vector4Input(const std::string& name, Vector4& target, float speed, float _min, float _max, bool enableLog)
      {
          return ImGui::DragFloat4(name.c_str(), &target.x, speed, _min, _max, "%.03f", enableLog ? ImGuiSliderFlags_Logarithmic : 0);
      }

      bool FloatInput(const std::string& name, float& target, float speed, float _min, float _max, bool enableLog)
      {
         return ImGui::DragFloat(name.c_str(), &target, 0.1f, _min, _max, "%.03f", enableLog ? ImGuiSliderFlags_Logarithmic : 0);
      }
   }
}