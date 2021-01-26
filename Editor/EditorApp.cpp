#include "EditorApp.h"
#include "Core/Context.h"
#include "Core/Engine.h"
#include "Core/Config.h"
#include "GameFramework/World.h"
#include "Core/imguiHelper.h"
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
               PushLayer(m_menuBarLayer);

               LoadEditorConfig();
               InitGUIStyle();

               m_gameViewLayer = new GameViewLayer(context);
               if (!m_gameViewLayer->Init())
               {
                  return false;
               }

               PushLayer(m_gameViewLayer);
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
            editorConfigJson["GUI"] = guiConfig;

            configSys->SaveConfig(TEXT("Editor"));
         }
      }

      void EditorApp::InitGUIStyle()
      {
         SetupImGuiStyle(EGUIStyle::Cherry);
      }
   }
}