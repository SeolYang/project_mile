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

      private:
         void FileMenu();
         void RendererConfig();

      private:
         World* m_world;
         Window* m_window;
         RendererDX11* m_renderer;
         ConfigSystem* m_configSys;
         bool m_bIsRendererConfigOpened;

         Vector2 m_referenceResolution;

      };
   }
}