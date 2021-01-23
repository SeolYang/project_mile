#pragma once
#include "Core/Application.h"

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
         ~EditorApp();

         virtual bool Init() override;

         void LoadEditorConfig();
         void SaveEditorConfig();

      private:
         void InitGUIStyle();

      private:
         Engine* m_engineInstance;
         WorldHierarchyLayer* m_worldHierarchyLayer;
         MenuBarLayer* m_menuBarLayer;
         GameViewLayer* m_gameViewLayer;
         bool m_bIsDarkStyle;
         float m_guiAlpha;

      };
   }
}