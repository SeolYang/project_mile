#pragma once
#include "Core/Layer.h"

namespace Mile
{
   class World;
   class Window;
   namespace Editor
   {
      class MenuBarLayer : public Layer
      {
      public:
         MenuBarLayer(Context* context);
         virtual void OnIMGUIRender();

         void SetTargetWorld(World* target) { m_world = target; }
         void SetWindow(Window* window) { m_window = window; }

      private:
         void FileMenu();

      private:
         World* m_world;
         Window* m_window;

      };
   }
}