#pragma once
#include "Core/Application.h"

namespace Mile
{
   class Engine;
   namespace Editor
   {
      class SceneHierarchyLayer;
      class EditorApp : public Application
      {
      public:
         EditorApp(Context* context);
         ~EditorApp();

         virtual bool Init() override;

      private:
         Engine* m_engineInstance;
         SceneHierarchyLayer* m_sceneHierarchyLayer;

      };
   }
}