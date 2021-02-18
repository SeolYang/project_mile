#include "Core/ImGuiHelper.h"
#include "Math/MathMinimal.h"
#include "GameFramework/Transform.h"

namespace Mile
{
   namespace GUI
   {
      bool Vector3Input(const std::string& name, Vector3& target)
      {
         return ImGui::DragFloat3(name.c_str(), &target.x, 0.1f, -FLT_MAX, FLT_MAX);
      }
   }
}