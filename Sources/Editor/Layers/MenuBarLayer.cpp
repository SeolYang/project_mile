#include "Layers/MenuBarLayer.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include "Core/Engine.h"
#include "Core/Timer.h"
#include "Core/ImGuiHelper.h"
#include "Resource/ResourceManager.h"
#include "Resource/Texture2D.h"
#include "GameFramework/World.h"
#include "Rendering/RendererDX11.h"
#include "Rendering/RendererPBR.h"
#include "Rendering/Texture2dDX11.h"
#include "Rendering/GPUProfiler.h"
#include "GraphicsDebugWindow.h"
#include "EditorApp.h"

namespace Mile
{
   namespace Editor
   {
      DEFINE_LOG_CATEGORY(MileMenuBarLayer);

      MenuBarLayer::MenuBarLayer(Context* context) :
         m_world(nullptr),
         m_window(nullptr),
         m_renderer(nullptr),
         m_configSys(nullptr),
         m_editorApp(nullptr),
         m_fps(0),
         m_bIsEngineConfigOpend(true),
         m_bIsEditorConfigOpend(true),
         m_bIsRendererConfigOpened(true),
         m_bIsGPUProfilerOpened(true),
         m_bIsGraphicsDebugWindowOpened(true),
         Layer(context, TEXT("MenuBarLayer"))
      {
      }

      void MenuBarLayer::OnIMGUIRender()
      {
         OPTICK_EVENT();
         if (ImGui::BeginMainMenuBar())
         {
            if (ImGui::BeginMenu("File"))
            {
               FileMenu();
               ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Config"))
            {
               if (ImGui::MenuItem("Engine", nullptr, &m_bIsEngineConfigOpend))
               {
                  m_fps = Engine::GetInstance()->GetMaxFPS();
               }

               if (ImGui::MenuItem("Editor", nullptr, &m_bIsEditorConfigOpend))
               {
                  /* Empty */
               }

               if (ImGui::MenuItem("Renderer", NULL, &m_bIsRendererConfigOpened))
               {
                  /* Empty */
               }

               ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
               ImGui::MenuItem("GPU Profiler", nullptr, &m_bIsGPUProfilerOpened);
               ImGui::MenuItem("Graphics Debugger", nullptr, &m_bIsGraphicsDebugWindowOpened);
               ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();

            if (m_bIsEngineConfigOpend)
            {
               EngineConfig();
            }

            if (m_bIsEditorConfigOpend)
            {
               EditorConfig();
            }

            if (m_bIsRendererConfigOpened)
            {
               RendererConfig();
            }

            if (m_bIsGPUProfilerOpened)
            {
               GPUProfiler();
            }

            if (m_bIsGraphicsDebugWindowOpened)
            {
               m_graphicsDebugWindow.OnGUI();
            }
         }
      }

      void MenuBarLayer::FileMenu()
      {
         OPENFILENAME openFileName;
         TCHAR szFile[MAX_FILE_LENGTH] = { 0 };
         ZeroMemory(&openFileName, sizeof(openFileName));

         if (ImGui::MenuItem("New World"))
         {
            /* New World **/
            if (m_world != nullptr)
            {
               m_world->Clear();
            }
         }
         if (ImGui::MenuItem("Open World", "Ctrl+O"))
         {
            /* Open World **/
            if (m_window != nullptr)
            {
               openFileName.lStructSize = sizeof(openFileName);
               openFileName.hwndOwner = (HWND)m_window->GetHandle();
               openFileName.lpstrTitle = TEXT("Select world file to open.");
               openFileName.lpstrFile = szFile;
               openFileName.nMaxFile = sizeof(szFile);
               openFileName.lpstrFilter = TEXT("World\0*.world\0All\0*.*\0");
               openFileName.nFilterIndex = 1;
               openFileName.nMaxFileTitle = 0;
               openFileName.lpstrInitialDir = NULL;
               openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

               if (GetOpenFileName(&openFileName) == TRUE)
               {
                  String filePath = String(openFileName.lpstrFile);
                  ME_LOG(MileMenuBarLayer, Log, String(TEXT("Open world from ")) + filePath);
                  if (m_world != nullptr)
                  {
                     m_world->LoadFrom(filePath);
                  }
               }
            }
         }

         bool failedToSave = false;
         if (ImGui::MenuItem("Save World", "Ctrl+S"))
         {
            /* Save World **/
            if (m_world != nullptr)
            {
               failedToSave = !m_world->Save();
            }
         }
         if (ImGui::MenuItem("Save World As..") || failedToSave)
         {
            /* Save World As .. **/
            if (m_window != nullptr)
            {
               openFileName.lStructSize = sizeof(openFileName);
               openFileName.hwndOwner = (HWND)m_window->GetHandle();
               openFileName.lpstrTitle = TEXT("Save As..");
               openFileName.lpstrFile = szFile;
               openFileName.nMaxFile = sizeof(szFile);
               openFileName.lpstrFilter = TEXT("World\0*.world\0All\0*.*\0");
               openFileName.nFilterIndex = 0;
               openFileName.nMaxFileTitle = 0;
               openFileName.lpstrInitialDir = NULL;
               openFileName.Flags = OFN_NOCHANGEDIR;

               if (GetSaveFileName(&openFileName) == TRUE)
               {
                  String filePath = String(openFileName.lpstrFile) + String(TEXT(".")) + String(openFileName.lpstrFilter);
                  ME_LOG(MileMenuBarLayer, Log, String(TEXT("Save world as ")) + filePath);
                  if (m_world != nullptr)
                  {
                     m_world->SaveTo(filePath);
                  }
               }
            }
         }
      }

      void MenuBarLayer::EngineConfig()
      {
         ImGui::Begin("Engine Configuration");
         Engine* engine = Engine::GetInstance();
         if (engine != nullptr)
         {
            ImGui::Text("Maximum framerate per second");
            ImGui::InputInt("Target FPS", &m_fps);
            if (ImGui::Button("Apply"))
            {
               engine->SetMaxFPS(m_fps);
               m_fps = engine->GetMaxFPS();
            }

            ImGui::Text((std::string("Current FPS : ") + std::to_string(engine->GetCurrentFPS())).c_str());
            ImGui::Text((std::string("Current Deltatime : ") + std::to_string(engine->GetTimer()->GetDeltaTime())).c_str());

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            if (ImGui::Button("Save Configs"))
            {
               SaveEngineConfig();
            }
         }
         ImGui::End();
      }

      void MenuBarLayer::SaveEngineConfig()
      {
         Engine* engine = Engine::GetInstance();
         if (engine != nullptr)
         {
            engine->SaveConfig();
         }
      }

      void MenuBarLayer::RendererConfig()
      {
         ImGui::Begin("Renderer Configuration");
         if (m_renderer != nullptr)
         {
            /** Display */
            if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen))
            {
               //ImGui::Spacing();
               if (ImGui::TreeNode("Render Resolution"))
               {
                  if (m_renderRes.SizeSquared() == 0.0f)
                  {
                     m_renderRes = m_renderer->GetRenderResolution();
                  }
                  ImGui::InputFloat2("", m_renderRes.elements, "%.0f");
                  ImGui::SameLine();
                  if (ImGui::Button("Apply"))
                  {
                     m_renderer->SetRenderResolution(m_renderRes);
                  }

                  ImGui::TreePop();
               }

               ImGui::Spacing();

               bool bIsVsyncEnabled = m_renderer->IsVsyncEnabled();
               if (ImGui::Checkbox("Vsync", &bIsVsyncEnabled))
               {
                  m_renderer->SetVsync(bIsVsyncEnabled);
               }

               ImGui::Spacing();
               ImGui::Separator();
               ImGui::Spacing();
            }

            auto pbrRenderer = dynamic_cast<RendererPBR*>(m_renderer);
            if (pbrRenderer != nullptr)
            {
               if (ImGui::CollapsingHeader("RendererPBR", ImGuiTreeNodeFlags_DefaultOpen))
               {
                  ImGui::Spacing();
                  if (ImGui::TreeNode("Tone Mapping"))
                  {
                     auto& toneMappingParams = pbrRenderer->GetToneMappingParams();
                     ImGui::SliderFloat("Exposure", &toneMappingParams.ExposureFactor, 0.0f, 64.0f);
                     ImGui::SliderFloat("Gamma", &toneMappingParams.GammaFactor, 0.0f, 10.0f);
                     ImGui::TreePop();
                  }

                  if (ImGui::TreeNode("Indirect Lighting"))
                  {
                     float& ambientIntensity = pbrRenderer->GetAmbientIntensty();
                     ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.0f, 1.0f);
                     ImGui::TreePop();
                  }

                  if (ImGui::TreeNode("Screen Space Ambient Occlusion(SSAO)"))
                  {
                     bool& bSSAOEnabled = pbrRenderer->SSAOEnabled();
                     ImGui::Checkbox("Enable SSAO", &bSSAOEnabled);
                     if (bSSAOEnabled)
                     {
                        auto& ssaoParams = pbrRenderer->GetSSAOParams();
                        ImGui::SliderFloat("SSAO Radius", &ssaoParams.Radius, 0.0f, 32.0f);
                        ImGui::SliderFloat("SSAO Bias", &ssaoParams.Bias, -16.0f, 16.0f);
                        ImGui::SliderFloat("SSAO Magnitude", &ssaoParams.Magnitude, 0.0f, 16.0f);
                     }
                     ImGui::TreePop();
                  }

                  if (ImGui::TreeNode("Bloom"))
                  {
                     BloomParams& bloomParams = pbrRenderer->GetBloomParams();
                     ImGui::SliderInt("BlurAmount", (int*)&bloomParams.BlurAmount, 0, 16);
                     ImGui::SliderFloat("Intensity", &bloomParams.Intensity, 0.0f, 30.0f);
                     ImGui::SliderFloat("Brightness Threshold", &bloomParams.BrightnessThreshold, 0.0f, 1.0f);
                     ImGui::TreePop();
                  }

                  ImGui::Spacing();
                  ImGui::Separator();
                  ImGui::Spacing();
               }
            }
         }
         else
         {
            ImGui::Text("Renderer does not exist or outdated.");
            ME_LOG(MileMenuBarLayer, Fatal, TEXT("Renderer doest not exist!"));
         }

         if (ImGui::Button("Save Configs"))
         {
            SaveRendererConfig();
         }

         ImGui::End();
      }

      void MenuBarLayer::SaveRendererConfig()
      {
         if (m_renderer != nullptr)
         {
            m_renderer->SaveConfig();
         }
      }

      void MenuBarLayer::GPUProfiler()
      {
         Engine* engine = Engine::GetInstance();
         auto& profiler = Engine::GetRenderer()->GetProfiler();
         ImGui::Begin("GPU Profiler");
         ImGui::Spacing();
         std::string fpsStr = (std::string("FPS : ") + std::to_string(engine->GetCurrentFPS()));
         ImGui::Text(fpsStr.c_str());

         std::string frameCountStr = std::string("Rendered Frames : ") + std::to_string(profiler.GetCurrentFrame());
         ImGui::Text(frameCountStr.c_str());

         std::string drawCallsStr = std::string("Draw Calls : ") + std::to_string(profiler.GetLatestDrawCalls());
         ImGui::Text(drawCallsStr.c_str());

         std::string verticesStr = std::string("Verts : ") + std::to_string(profiler.GetLatestVertices());
         ImGui::SameLine();
         ImGui::Spacing();
         ImGui::SameLine();
         ImGui::Text(verticesStr.c_str());

         std::string trianglesStr = std::string("Tris : ") + std::to_string(profiler.GetLatestTriangles());
         ImGui::SameLine();
         ImGui::Spacing();
         ImGui::SameLine();
         ImGui::Text(trianglesStr.c_str());

         std::string deltaTimeStr = (std::string("Deltatime : ") + std::to_string(engine->GetTimer()->GetDeltaTimeMS())) + std::string(" ms");
         ImGui::Text(deltaTimeStr.c_str());
         ImGui::Spacing();

         if (ImGui::CollapsingHeader("Profiles", ImGuiTreeNodeFlags_DefaultOpen))
         {
            ImGui::Spacing();
            int latency = (int)profiler.GetQueryLatency();
            if (ImGui::SliderInt("Latency (Frame) : ", &latency, 0, engine->GetMaxFPS()))
            {
               profiler.SetQueryLatency(latency);
            }
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            if (ImGui::BeginChild("ProfileDatas", ImVec2(0, 0), true))
            {
               ImGui::Columns(6, nullptr);
               {
                  const auto& profileDatas = profiler.GetProfileTimes();

                  ImGui::Text("Name");
                  ImGui::Text("Overall");
                  for (const auto& data : profileDatas)
                  {
                     ImGui::Text(data.first.c_str());
                  }

                  ImGui::NextColumn();

                  ImGui::Text("Recent(ms)");
                  ImGui::Text(Mile::Formatting("%.03f ms", profiler.GetProfileOverallTime()).c_str());
                  for (const auto& data : profileDatas)
                  {
                     const auto& accData = data.second;
                     std::string profileStr = Mile::Formatting("%.03f ms", accData.RecentElapsed);
                     ImGui::Text(profileStr.c_str());
                  }

                  ImGui::NextColumn();

                  ImGui::Text("Min(ms)");
                  ImGui::Text(" ");
                  for (const auto& data : profileDatas)
                  {
                     const auto& accData = data.second;
                     std::string profileStr = Mile::Formatting("%.03f ms", accData.MinTime);
                     ImGui::Text(profileStr.c_str());
                  }

                  ImGui::NextColumn();

                  ImGui::Text("Max(ms)");
                  ImGui::Text(" ");
                  for (const auto& data : profileDatas)
                  {
                     const auto& accData = data.second;
                     std::string profileStr = Mile::Formatting("%.03f ms", accData.MaxTime);
                     ImGui::Text(profileStr.c_str());
                  }

                  ImGui::NextColumn();

                  ImGui::Text("Avg(ms)");
                  ImGui::Text(" ");
                  for (const auto& data : profileDatas)
                  {
                     const auto& accData = data.second;
                     std::string profileStr = Mile::Formatting("%.03f ms", accData.AvgTime);
                     ImGui::Text(profileStr.c_str());
                  }

                  ImGui::NextColumn();

                  ImGui::Text("Samples");
                  ImGui::Text(" ");
                  for (const auto& data : profileDatas)
                  {
                     const auto& accData = data.second;
                     std::string profileStr = Mile::Formatting("%.0f", accData.Samples);
                     ImGui::Text(profileStr.c_str());
                  }
               }
               ImGui::EndChild();
            }
            ImGui::PopStyleColor();
         }

         ImGui::End();
      }

      void MenuBarLayer::EditorConfig()
      {
         ImGui::Begin("Editor Configurations");

         ImGui::Spacing();
         ImGui::Separator();
         ImGui::Spacing();

         const char* items[] = { "Mint", "BlueGrey", "Cherry", "DarkCharcoal", "LightBlue", "ClassicSteam", "GoldBlack", "UE4Like" };
         static const char* currentItem = items[m_editorApp->GetTheme()];

         if (ImGui::BeginCombo("Theme", currentItem))
         {
            for (UINT32 idx = 0; idx < IM_ARRAYSIZE(items); ++idx)
            {
               bool bSelected = (currentItem == items[idx]);
               if (ImGui::Selectable(items[idx], bSelected))
               {
                  currentItem = items[idx];
                  m_editorApp->SetTheme((EGUIStyle)idx);
               }
               if (bSelected)
               {
                  ImGui::SetItemDefaultFocus();
               }
            }

            ImGui::EndCombo();
         }

         ImGui::Spacing();
         ImGui::Separator();
         ImGui::Spacing();

         if (ImGui::Button("Save Configs"))
         {
            SaveEditorConfig();
         }

         ImGui::End();
      }

      void MenuBarLayer::SaveEditorConfig()
      {
         if (m_editorApp != nullptr)
         {
            m_editorApp->SaveConfig();
         }
      }
   }
}