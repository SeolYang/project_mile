#include "EditorApp.h"
#include "Core/Context.h"
#include "Core/Engine.h"
#include "Core/Config.h"
#include "Core/imguiHelper.h"
#include "Layers/WorldHierarchyLayer.h"
#include "Layers/MenuBarLayer.h"

namespace Mile
{
   namespace Editor
   {
      EditorApp::EditorApp(Context* context) :
         m_engineInstance(nullptr),
         m_worldHierarchyLayer(nullptr),
         m_menuBarLayer(nullptr),
         m_bIsDarkStyle(true),
         m_guiAlpha(0.8f),
         Application(context, TEXT("MileEditor"))
      {
      }

      EditorApp::~EditorApp()
      {
         m_engineInstance = nullptr;
         m_worldHierarchyLayer = nullptr;
         m_menuBarLayer = nullptr;
      }

      bool EditorApp::Init()
      {
         if (Application::Init())
         {
            Context* context = GetContext();
            m_engineInstance = context->GetSubSystem<Engine>();
            if (m_engineInstance != nullptr)
            {
               World* world = m_engineInstance->GetWorld();
               Window* window = m_engineInstance->GetWindow();
               RendererDX11* renderer = m_engineInstance->GetRenderer();
               ConfigSystem* configSys = m_engineInstance->GetConfigSystem();
               m_worldHierarchyLayer = new WorldHierarchyLayer(context);
               m_worldHierarchyLayer->SetTargetWorld(world);
               PushLayer(m_worldHierarchyLayer);

               m_menuBarLayer = new MenuBarLayer(context);
               m_menuBarLayer->SetTargetWorld(world);
               m_menuBarLayer->SetWindow(window);
               m_menuBarLayer->SetRenderer(renderer);
               m_menuBarLayer->SetConfigSystem(configSys);
               PushLayer(m_menuBarLayer);

               LoadEditorConfig();
               InitGUIStyle();

               return true;
            }
         }

         return false;
      }

      void EditorApp::LoadEditorConfig()
      {
         if (m_engineInstance != nullptr)
         {
            ConfigSystem* configSys = m_engineInstance->GetConfigSystem();
            if (configSys->LoadConfig(TEXT("Editor")))
            {
               auto& editorConfig = configSys->GetConfig(TEXT("Editor"));

               /* GUI Config **/
               json guiConfig = GetValueSafelyFromJson(editorConfig.second, "GUI", json());
               m_bIsDarkStyle = GetValueSafelyFromJson(guiConfig, "IsDarkStyle", true);
               m_guiAlpha = GetValueSafelyFromJson(guiConfig, "GuiAlpha", 0.75f);
            }
         }
      }

      void EditorApp::SaveEditorConfig()
      {
         if (m_engineInstance != nullptr)
         {
            ConfigSystem* configSys = m_engineInstance->GetConfigSystem();
            auto& editorConfig = configSys->GetConfig(TEXT("Editor"));
            json& editorConfigJson = editorConfig.second;

            /* GUI Config **/
            json guiConfig;
            guiConfig["IsDarkStyle"] = m_bIsDarkStyle;
            guiConfig["GuiAlpha"] = m_guiAlpha;
            editorConfigJson["GUI"] = guiConfig;

            configSys->SaveConfig(TEXT("Editor"));
         }
      }

      void EditorApp::InitGUIStyle()
      {
         SetupImGuiStyle(m_bIsDarkStyle, m_guiAlpha);
      }
   }
}