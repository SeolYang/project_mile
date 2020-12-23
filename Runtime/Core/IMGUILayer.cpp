#include "Core/IMGUILayer.h"
#include "Core/Context.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include "Rendering/RendererDX11.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace Mile
{
   IMGUILayer::IMGUILayer(Context* context) :
      Layer(context, TEXT("IMGUILayer"))
   {
   }

   void IMGUILayer::OnAttach()
   {
      Context* context = GetContext();
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO(); (void)io;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

      io.Fonts->AddFontFromFileTTF("Contents/Fonts/NanumGothicBold.ttf", 18.0f);
      io.FontDefault = io.Fonts->AddFontFromFileTTF("Contents/Fonts/NanumGothic.ttf", 18.0f);

      ImGui::StyleColorsDark();
      //ImGui::StyleColorsClassic();

      ImGuiStyle& style = ImGui::GetStyle();
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
         style.WindowRounding = 0.0f;
         style.Colors[ImGuiCol_WindowBg].w = 1.0f;
      }

      auto window = context->GetSubSystem<Window>();
      if (window == nullptr)
      {
         MELog(context, TEXT("IMGUILayer"), ELogType::FATAL, TEXT("Window Subsystem does not exist!"));
         return;
      }
      else
      {
         auto renderer = context->GetSubSystem<RendererDX11>();
         if (renderer == nullptr)
         {
            MELog(context, TEXT("IMGUILayer"), ELogType::FATAL, TEXT("Renderer Subsystem does not exist!"));
            return;
         }
         else
         {
            ImGui_ImplWin32_Init(window->GetHandle());
            ImGui_ImplDX11_Init(renderer->GetDevice(), renderer->GetImmediateContext());
         }
      }
   }

   void IMGUILayer::OnDetach()
   {
      ImGui_ImplDX11_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
   }

   void IMGUILayer::Begin()
   {
      ImGui_ImplDX11_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();
   }

   void IMGUILayer::End()
   {
      Context* context = GetContext();
      auto renderer = context->GetSubSystem<RendererDX11>();
      auto window = context->GetSubSystem<Window>();
      ImGuiIO& io = ImGui::GetIO();
      io.DisplaySize = ImVec2((float)window->GetResWidth(), (float)window->GetResHeight());

      ImGui::Render();
      auto immediateContext = renderer->GetImmediateContext();
      renderer->SetBackbufferAsRenderTarget(*immediateContext);
      renderer->ClearDepthStencil(*immediateContext);
      ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
         ImGui::UpdatePlatformWindows();
         ImGui::RenderPlatformWindowsDefault();
      }
   }
}
