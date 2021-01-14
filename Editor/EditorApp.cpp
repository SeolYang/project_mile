#include "EditorApp.h"
#include "Core/Context.h"
#include "Core/Engine.h"
#include "Layers/SceneHierarchyLayer.h"

namespace Mile
{
   namespace Editor
   {
      EditorApp::EditorApp(Context* context) :
         m_engineInstance(nullptr),
         m_sceneHierarchyLayer(nullptr),
         Application(context, TEXT("MileEditor"))
      {
      }

      EditorApp::~EditorApp()
      {
         m_engineInstance = nullptr;
         m_sceneHierarchyLayer = nullptr;
      }

      bool EditorApp::Init()
      {
         if (Application::Init())
         {
            Context* context = GetContext();
            m_engineInstance = context->GetSubSystem<Engine>();
            if (m_engineInstance != nullptr)
            {
               m_sceneHierarchyLayer = new SceneHierarchyLayer(context);
               m_sceneHierarchyLayer->SetTargetWorld(m_engineInstance->GetWorld());
               PushLayer(m_sceneHierarchyLayer);
               return true;
            }
         }

         return false;
      }
   }
}