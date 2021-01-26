#pragma once
#include "Core/Application.h"
#include "Core/imguiHelper.h"

#define EDITOR_CONFIG TEXT("Editor")
#define EDITOR_CONFIG_THEME "Theme"

namespace Mile
{
   class Engine;
   class World;
   class Window;
   class ConfigSystem;
   class Entity;
   class CameraComponent;
   class RenderTexture;
   namespace Editor
   {
      DECLARE_LOG_CATEGORY_EXTERN(MileEditor, Log);

      class WorldHierarchyLayer;
      class MenuBarLayer;
      class GameViewLayer;
      class EditorApp : public Application
      {
      public:
         EditorApp(Context* context);
         virtual ~EditorApp();

         virtual bool Init() override;

         EGUIStyle GetTheme() const { return m_theme; }
         void SetTheme(EGUIStyle theme);

         virtual void LoadConfig();
         virtual void SaveConfig();

      private:
         Engine* m_engineInstance;
         WorldHierarchyLayer* m_worldHierarchyLayer;
         MenuBarLayer* m_menuBarLayer;
         GameViewLayer* m_gameViewLayer;

         EGUIStyle m_theme;

      };
   }
}