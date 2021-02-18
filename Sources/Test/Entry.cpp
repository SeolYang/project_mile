#include "Core/Context.h"
#include "Core/Engine.h"
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
#include "RotateComponent.h"
#include "InputTestComponent.h"
#include "TestApp.h"

using namespace Mile;

int main()
{
   auto context = new Context();
   auto app = new TestApp(context);
   auto engine = new Engine(context, app);
   int execute = 1;

   if (engine->Init())
   {
      auto world = Engine::GetWorld();
      auto resMng = Engine::GetResourceManager();
      auto renderer = Engine::GetRenderer();
      auto inputMng = Engine::GetInputManager();

      inputMng->MapAction(EInputKey::W, TEXT("Forward"));

      //Texture2D* helipadGoldenHourHDR = resMng->Load<Texture2D>(TEXT("Contents/Textures/Helipad_GoldenHour/LA_Downtown_Helipad_GoldenHour_3k.hdr"));
      //renderer->SetEquirectangularMap(helipadGoldenHourHDR);

      //Texture2D* newPortHDR = resMng->Load<Texture2D>(TEXT("Contents/Textures/Newport_Loft/Newport_Loft_Ref.hdr"));
      //renderer->SetEquirectangularMap(newPortHDR);

      //Texture2D* walkOfFameHDR = resMng->Load<Texture2D>(TEXT("Contents/Textures/Walk_Of_Fame/Mans_Outside_2k.hdr"));
      //renderer->SetEquirectangularMap(walkOfFameHDR);

      //Texture2D* winterForestHDR = resMng->Load<Texture2D>(TEXT("Contents/Textures/Winter_Forest/WinterForest_Ref.hdr"));
      //renderer->SetEquirectangularMap(winterForestHDR);

      //Texture2D* barcelonaRooftopsHDR = resMng->Load<Texture2D>(TEXT("Contents/Textures/Barcelona_Rooftops/Barce_Rooftop_C_3k.hdr"));
      //renderer->SetEquirectangularMap(barcelonaRooftopsHDR);

      //Texture2D* snowMachineHDR = resMng->Load<Texture2D>(TEXT("Contents/Textures/snow_machine/test8_Ref.hdr"));
      //renderer->SetEquirectangularMap(snowMachineHDR);

      renderer->SetVsync(false);
      execute = engine->Execute();
   }

   SafeDelete(context);
   return execute;
}