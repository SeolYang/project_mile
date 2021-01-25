#include "Editor.h"
#include "Core/Context.h"
#include "Core/Engine.h"

/*TEST INCLUDES*/
#include "Core/Logger.h"
#include "Core/InputManager.h"
#include "Core/Application.h"
#include "GameFramework/World.h"
#include "GameFramework/Entity.h"
#include "Component/CameraComponent.h"
#include "Component/MeshRenderComponent.h"
#include "Component/LightComponent.h"
#include "Resource/ResourceManager.h"
#include "Resource/Model.h"
#include "Resource/ModelLoader.h"
#include "Resource/Material.h"
#include "Resource/Texture2D.h"
#include "Rendering/Cube.h"
#include "Rendering/Quad.h"
#include "Rendering/RendererDX11.h"
#include "Math/Vector3.h"
#include "MT/ThreadPool.h"
#include "Core/Delegate.h"
using namespace Mile;
/**************/

#ifdef _DEBUG
#include <iostream>
#include <crtdbg.h>
#endif

#include "EditorApp.h"

int main()
{
#ifdef _DEBUG
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   //_CrtSetBreakAlloc(70953);
#endif
   int execute = 1;
   auto context = new Mile::Context();
   auto app = new Mile::Editor::EditorApp(context);
   auto engine = new Mile::Engine(context, app);

   if (engine->Init())
   {
      /* TEST CODES */
      auto world = Engine::GetWorld();
      auto resMng = Engine::GetResourceManager();
      auto renderer = Engine::GetRenderer();
      auto inputMng = Engine::GetInputManager();

      inputMng->MapAction(EInputKey::W, TEXT("Forward"));
      /* TEST CODES END*/

      execute = engine->Execute();
   }

   SafeDelete(context);
   return execute;
}