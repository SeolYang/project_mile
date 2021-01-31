#include "EditorApp.h"
#include "Core/Context.h"
#include "Core/Engine.h"
#include "Core/Config.h"
#include "GameFramework/World.h"
#include "Layers/WorldHierarchyLayer.h"
#include "Layers/MenuBarLayer.h"
#include "Layers/GameViewLayer.h"
#include "Resource/ResourceManager.h"
#include "Resource/RenderTexture.h"

namespace Mile
{
   namespace Editor
   {
      DEFINE_LOG_CATEGORY(MileEditor);

      EditorApp::EditorApp(Context* context) :
         m_engineInstance(nullptr),
         m_worldHierarchyLayer(nullptr),
         m_menuBarLayer(nullptr),
         m_gameViewLayer(nullptr),
         m_theme(EGUIStyle::LightBlue),
         Application(context, TEXT("MileEditor"))
      {
      }

      EditorApp::~EditorApp()
      {
         m_engineInstance = nullptr;
         m_worldHierarchyLayer = nullptr;
         m_menuBarLayer = nullptr;
         m_gameViewLayer = nullptr;
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
               ResourceManager* resMng = Engine::GetResourceManager();
               m_worldHierarchyLayer = new WorldHierarchyLayer(context);
               m_worldHierarchyLayer->SetTargetWorld(world);
               PushLayer(m_worldHierarchyLayer);

               m_menuBarLayer = new MenuBarLayer(context);
               m_menuBarLayer->SetTargetWorld(world);
               m_menuBarLayer->SetWindow(window);
               m_menuBarLayer->SetRenderer(renderer);
               m_menuBarLayer->SetConfigSystem(configSys);
               m_menuBarLayer->SetEditorApp(this);
               PushLayer(m_menuBarLayer);

               m_gameViewLayer = new GameViewLayer(context);
               if (!m_gameViewLayer->Init())
               {
                  return false;
               }
               PushLayer(m_gameViewLayer);

               LoadConfig();
               ME_LOG(MileEditor, Log, TEXT("Editor application initialized."));
               return true;
            }
         }

         ME_LOG(MileEditor, Fatal, TEXT("Failed to initialize editor application!"));
         return false;
      }

      void EditorApp::SetTheme(EGUIStyle theme)
      {
         m_theme = theme;
         SetupImGuiStyle(theme);
      }

      void EditorApp::LoadConfig()
      {
         if (m_engineInstance != nullptr)
         {
            ConfigSystem* configSys = m_engineInstance->GetConfigSystem();
            if (configSys->LoadConfig(EDITOR_CONFIG))
            {
               auto& editorConfig = configSys->GetConfig(EDITOR_CONFIG);
               EGUIStyle theme = GetValueSafelyFromJson(editorConfig.second, EDITOR_CONFIG_THEME, EGUIStyle::LightBlue);
               SetTheme(theme);

               ME_LOG(MileEditor, Log, TEXT("Editor configurations loaded."));
               return;
            }
         }

         ME_LOG(MileEditor, Fatal, TEXT("Failed to load Editor configurations!"));
      }

      void EditorApp::SaveConfig()
      {
         if (m_engineInstance != nullptr)
         {
            ConfigSystem* configSys = m_engineInstance->GetConfigSystem();
            if (configSys->LoadConfig(EDITOR_CONFIG))
            {
               auto& editorConfig = configSys->GetConfig(EDITOR_CONFIG);
               editorConfig.second[EDITOR_CONFIG_THEME] = (UINT32)GetTheme();
               if (configSys->SaveConfig(TEXT("Editor")))
               {
                  ME_LOG(MileEditor, Log, TEXT("Editor configurations saved."));
                  return;
               }
            }
         }

         ME_LOG(MileEditor, Fatal, TEXT("Failed to save Editor configurations!"));
      }
   }
}