#pragma once
#include "Core/Application.h"
#include "Core/imguiHelper.h"

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
      class WorldHierarchyLayer;
      class MenuBarLayer;
      class GameViewLayer;
      class EditorApp : public Application
      {
      public:
         EditorApp(Context* context);
         virtual ~EditorApp();

         virtual bool Init() override;

         void LoadEditorConfig();

         EGUIStyle GetTheme() const { return m_theme; }
         void SetTheme(EGUIStyle theme);

      private:
         Engine* m_engineInstance;
         WorldHierarchyLayer* m_worldHierarchyLayer;
         MenuBarLayer* m_menuBarLayer;
         GameViewLayer* m_gameViewLayer;

         EGUIStyle m_theme;

      };
   }
}