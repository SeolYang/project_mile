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
#include "Component/CameraComponent.h"

namespace Mile
{
   namespace Editor
   {
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

               LoadEditorConfig();
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

               EGUIStyle theme = GetValueSafelyFromJson(editorConfig.second, "Theme", EGUIStyle::LightBlue);
               SetTheme(theme);
            }
         }
      }

      void EditorApp::SetTheme(EGUIStyle theme)
      {
         m_theme = theme;
         SetupImGuiStyle(theme);
      }
   }
}