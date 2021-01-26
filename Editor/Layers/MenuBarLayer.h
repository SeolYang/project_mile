#pragma once
#include "Core/Layer.h"
#include "Core/Logger.h"
#include "Math/Vector2.h"

namespace Mile
{
   class World;
   class RendererDX11;
   class Window;
   class ConfigSystem;
   namespace Editor
   {
      class EditorApp;
      DECLARE_LOG_CATEGORY_EXTERN(MileMenuBarLayer, Log);

      class MenuBarLayer : public Layer
      {
      public:
         MenuBarLayer(Context* context);

         virtual void OnIMGUIRender();

         void SetTargetWorld(World* target) { m_world = target; }
         void SetWindow(Window* window) { m_window = window; }
         void SetRenderer(RendererDX11* renderer) { m_renderer = renderer; }
         void SetConfigSystem(ConfigSystem* configSys) { m_configSys = configSys; }
         void SetEditorApp(EditorApp* app) { m_editorApp = app; }

      private:
         void FileMenu();
         void RendererConfig();

         void EditorConfig();
         void SaveEditorConfig();

      private:
         World* m_world;
         Window* m_window;
         ConfigSystem* m_configSys;

         /** Editor Config */
         EditorApp* m_editorApp;
         bool m_bIsEditorConfigOpend;

         /** Renderer Config */
         RendererDX11* m_renderer;
         bool m_bIsRendererConfigOpened;
         Vector2 m_referenceResolution;

      };
   }
}