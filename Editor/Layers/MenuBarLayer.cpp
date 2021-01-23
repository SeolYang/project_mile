#include "Layers/MenuBarLayer.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include "Core/Engine.h"
#include "Core/imguiHelper.h"
#include "Resource/ResourceManager.h"
#include "Resource/Texture2D.h"
#include "GameFramework/World.h"
#include "Rendering/RendererDX11.h"
#include "Rendering/Texture2dDX11.h"

namespace Mile
{
   namespace Editor
   {
      MenuBarLayer::MenuBarLayer(Context* context) :
         m_world(nullptr),
         m_window(nullptr),
         m_renderer(nullptr),
         m_configSys(nullptr),
         m_bIsRendererConfigOpened(false),
         Layer(context, TEXT("MenuBarLayer"))
      {
      }

      void MenuBarLayer::OnIMGUIRender()
      {
         if (ImGui::BeginMainMenuBar())
         {
            if (ImGui::BeginMenu("File"))
            {
               FileMenu();
               ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Config"))
            {
               ImGui::MenuItem("Renderer", NULL, &m_bIsRendererConfigOpened);
               ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();

            if (m_bIsRendererConfigOpened)
            {
               RendererConfig();
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
                  MELog(GetContext(), TEXT("MenuBarLayer"), ELogType::MESSAGE, String(TEXT("Open world from ")) + filePath);
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
                  MELog(GetContext(), TEXT("MenuBarLayer"), ELogType::MESSAGE, String(TEXT("Save world as ")) + filePath);
                  if (m_world != nullptr)
                  {
                     m_world->SaveTo(filePath);
                  }
               }
            }
         }
      }

      void MenuBarLayer::RendererConfig()
      {
         ImGui::Begin("Renderer Configuration");
         WindowFocusedEffect(0, 255, 0);
         if (m_renderer != nullptr)
         {
            /** Post Process */
            ImGui::Text("Post-Process");

            float exposure = m_renderer->GetExposureFactor();
            if (ImGui::SliderFloat("Exposure", &exposure, 0.0f, 64.0f))
            {
               m_renderer->SetExposure(exposure);
            }

            float gamma = m_renderer->GetGammaFactor();
            if (ImGui::SliderFloat("Gamma", &gamma, 0.0f, 10.0f))
            {
               m_renderer->SetGammaFactor(gamma);
            }

            float ao = m_renderer->GetAmbientOcclusionFactor();
            if (ImGui::SliderFloat("Ambient Occlusion", &ao, 0.0f, 1.0f))
            {
               m_renderer->SetAmbientOcclusionFactor(ao);
            }

            EBloomType bloomType = m_renderer->GetBloomType();
            auto bloomTypeIdx = static_cast<int>(bloomType);
            constexpr auto BloomTypeEnumSize = static_cast<unsigned int>(EBloomType::EnumSize);
            const char* bloomTypeNames[BloomTypeEnumSize] = { "Box", "Gaussian", "None" };
            const char* bloomTypeName = (bloomTypeIdx >= 0 && bloomTypeIdx < BloomTypeEnumSize ?
               bloomTypeNames[bloomTypeIdx] : "Unknown");
            if (ImGui::SliderInt("Bloom Type", &bloomTypeIdx, 0, BloomTypeEnumSize - 1, bloomTypeName))
            {
               m_renderer->SetBloomType(static_cast<EBloomType>(bloomTypeIdx));
            }

            int gaussianBloomAmount = m_renderer->GetGaussianBloomAmount();
            float gaussianBloomIntensity = m_renderer->GetGaussianBloomIntensity();
            float gaussianBloomThreshold = m_renderer->GetGaussianBloomThreshold();
            float gaussianBloomDepthThreshold = m_renderer->GetGaussianBloomDepthThreshold();
            switch (static_cast<EBloomType>(bloomTypeIdx))
            {
            case EBloomType::Box:
               break;
            case EBloomType::Gaussian:
               ImGui::Text("Gaussian Bloom");
               if (ImGui::SliderInt("Amount", &gaussianBloomAmount, 0, 16))
               {
                  m_renderer->SetGaussianBloomAmount(gaussianBloomAmount);
               }
               if (ImGui::SliderFloat("Intensity", &gaussianBloomIntensity, 0.0f, 30.0f))
               {
                  m_renderer->SetGaussianBloomIntensity(gaussianBloomIntensity);
               }
               if (ImGui::SliderFloat("Threshold", &gaussianBloomThreshold, 0.0f, 1.0f))
               {
                  m_renderer->SetGaussianBloomThreshold(gaussianBloomThreshold);
               }
               break;
            }

            bool bIsSSAOEnabled = m_renderer->IsSSAOEnabled();
            float ssaoRadius = m_renderer->GetSSAORadius();
            float ssaoBias = m_renderer->GetSSAOBias();
            float ssaoMagnitude = m_renderer->GetSSAOMagnitude();
            if (ImGui::Checkbox("Enable SSAO", &bIsSSAOEnabled))
            {
               m_renderer->SetSSAOEanble(bIsSSAOEnabled);
            }
            if (bIsSSAOEnabled)
            {
               if(ImGui::SliderFloat("SSAO Radius", &ssaoRadius, 0.0f, 32.0f))
               {
                  m_renderer->SetSSAORadius(ssaoRadius);
               }
               if (ImGui::SliderFloat("SSAO Bias", &ssaoBias, -16.0f, 16.0f))
               {
                  m_renderer->SetSSAOBias(ssaoBias);
               }
               if (ImGui::SliderFloat("SSAO Magnitude", &ssaoMagnitude, 0.0f, 16.0f))
               {
                  m_renderer->SetSSAOMagnitude(ssaoMagnitude);
               }
            }

            /** Compute */
            ImGui::Text("Compute");
            bool bIsComputeIBLAsRealtime = m_renderer->IsComputeIBLAsRealtime();
            if (ImGui::Checkbox("Realtime IBL", &bIsComputeIBLAsRealtime))
            {
               m_renderer->SetComputeIBLAsRealtime(bIsComputeIBLAsRealtime);
            }

            /** For test! */
            ImGui::Text("Equirectangular Map");
            Texture2D* equirectangularMap = m_renderer->GetEquirectangularMap();
            if (ImGui::Button("Choose.."))
            {
               OPENFILENAME openFileName;
               TCHAR szFile[MAX_FILE_LENGTH] = { 0 };
               ZeroMemory(&openFileName, sizeof(openFileName));
               openFileName.lStructSize = sizeof(openFileName);
               openFileName.hwndOwner = (HWND)m_window->GetHandle();
               openFileName.lpstrTitle = TEXT("Select HDR file to open.");
               openFileName.lpstrFile = szFile;
               openFileName.nMaxFile = sizeof(szFile);
               openFileName.lpstrFilter = TEXT("Irradiance Map\0*.hdr\0All\0*.*\0");
               openFileName.nFilterIndex = 1;
               openFileName.nMaxFileTitle = 0;
               openFileName.lpstrInitialDir = NULL;
               openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

               if (GetOpenFileName(&openFileName) == TRUE)
               {
                  String filePath = String(openFileName.lpstrFile);
                  MELog(GetContext(), TEXT("MenuBarLayer"), ELogType::MESSAGE, String(TEXT("Open irradiance map from ")) + filePath);
                  equirectangularMap = Engine::GetResourceManager()->Load<Texture2D>(filePath);
                  m_renderer->SetEquirectangularMap(equirectangularMap);
               }
            }

            if (equirectangularMap != nullptr)
            {
               Texture2dDX11* mapRawTexture = equirectangularMap->GetRawTexture();
               ImVec2 mapRes{ (float)mapRawTexture->GetWidth(), (float)mapRawTexture->GetHeight() };
               ImVec2 outputRes{ 640.0f, 360.0f };
               ImGui::Text((std::string("Texture Path: ") + WString2String(equirectangularMap->GetPath())).c_str());
               ImGui::Text((std::string("Map Resolution : Width = ") + std::to_string(mapRes.x) + ", Height = " + std::to_string(mapRes.y)).c_str());
               ImGui::Image((void*)mapRawTexture->GetSRV(), outputRes);
            }
            else
            {
               ImGui::Text("Equirectangular Map does not exist!");
            }

            /** Display */
            ImGui::Text("Display");
            bool bIsVsyncEnabled = m_renderer->IsVsyncEnabled();
            if (ImGui::Checkbox("Vsync", &bIsVsyncEnabled))
            {
               m_renderer->SetVsync(bIsVsyncEnabled);
            }
         }
         else
         {
            MELog(GetContext(),
               TEXT("MenuBarLayer"),
               ELogType::FATAL,
               TEXT("Renderer doest not exist!"));
            ImGui::Text("Renderer does not exist or outdated.");
         }
         ImGui::End();
      }
   }
}