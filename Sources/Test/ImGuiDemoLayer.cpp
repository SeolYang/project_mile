#include "ImGuiDemoLayer.h"
#include "imgui.h"
#include "Core/Engine.h"
#include "Core/Timer.h"

ImGUIDemoLayer::ImGUIDemoLayer(Context* context) :
   Layer(context, TEXT("ImGUIDemoLayer"))
{
}

void ImGUIDemoLayer::OnIMGUIRender()
{
   static bool showDemo = false;
   static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
   if (showDemo)
      ImGui::ShowDemoWindow(&showDemo);

   {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Hello, world!");

      ImGui::Text("This is some useful text.");
      ImGui::Checkbox("Demo Window", &showDemo);

      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui::ColorEdit3("clear color", (float*)&clear_color);

      if (ImGui::Button("Button"))
         counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      float deltaTime = Engine::GetTimer()->GetDeltaTime();
      ImGui::Text("App dt %.4f ms", deltaTime);
      ImGui::End();
   }
}