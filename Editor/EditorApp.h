#pragma once
#include "Core/Application.h"

namespace Mile
{
   class Engine;
   class World;
   class Window;
   namespace Editor
   {
      class WorldHierarchyLayer;
      class MenuBarLayer;
      class EditorApp : public Application
      {
      public:
         EditorApp(Context* context);
         ~EditorApp();

         virtual bool Init() override;

      private:
         Engine* m_engineInstance;
         WorldHierarchyLayer* m_worldHierarchyLayer;
         MenuBarLayer* m_menuBarLayer;

      };
   }
}