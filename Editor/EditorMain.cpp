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
using namespace Mile;
/**************/

#include "EditorApp.h"

int main()
{
   auto context = new Mile::Context();
   auto app = new Mile::Editor::EditorApp(context);
   auto engine = new Mile::Engine(context, app);
   int execute = 1;

   if (engine->Init())
   {
      /* TEST CODES */
      auto world = Engine::GetWorld();
      auto resMng = Engine::GetResourceManager();
      auto renderer = Engine::GetRenderer();
      auto inputMng = Engine::GetInputManager();

      inputMng->MapAction(EInputKey::W, TEXT("Forward"));

      renderer->SetComputeIBLAsRealtime(false);
      renderer->SetAmbientOcclusionFactor(1.0f);
      renderer->SetGammaFactor(DEFAULT_GAMMA_FACTOR);
      renderer->SetExposure(1.0f);
      renderer->SetBloomType(EBloomType::Gaussian);
      renderer->SetGaussianBloomAmount(16);
      renderer->SetGaussianBloomIntensity(1.25f);
      renderer->SetGaussianBloomThreshold(0.8f);
      renderer->SetSSAOEanble(true);
      renderer->SetSSAORadius(1.5f);
      renderer->SetSSAOBias(0.02f);
      renderer->SetSSAOMagnitude(1.2f);
      renderer->SetVsync(false);

      Texture2D* iceLakeHDR = resMng->Load<Texture2D>(TEXT("Contents/Textures/Ice_Lake/Ice_Lake_Ref.hdr"));
      renderer->SetEquirectangularMap(iceLakeHDR);
      /* TEST CODES END*/

      execute = engine->Execute();
   }

   SafeDelete(context);
   return execute;
}