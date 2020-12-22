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

      Entity* cameraParent = world->CreateEntity(TEXT("CameraParent"));
      InputTestComponent* inputTest = cameraParent->AddComponent<InputTestComponent>();
      Transform* camParentTransform = cameraParent->GetTransform();
      RotateComponent* cameraParentRotation = cameraParent->AddComponent<RotateComponent>();
      Entity* camera = world->CreateEntity(TEXT("Camera"));
      CameraComponent* camComponent = camera->AddComponent<CameraComponent>();
      Transform* camTransform = camera->GetTransform();
      camComponent->SetNearPlane(0.1f);
      camComponent->SetFarPlane(100.0f);
      camComponent->SetFov(45.0f);
      camTransform->SetPosition(Vector3(0.0f, 0.5f, -4.0f));
      camTransform->SetRotation(Quaternion(-5.0f, Vector3(1.0f, 0.0f, 0.0f)));
      cameraParent->AttachChild(camera);

      Entity* mainLight = world->CreateEntity(TEXT("Main Light"));
      LightComponent* mainLightComponent = mainLight->AddComponent<LightComponent>();
      Transform* mainLightTransform = mainLight->GetTransform();
      mainLightComponent->SetLightType(ELightType::Point);
      mainLightComponent->SetRadiance(Vector3(5.0f, 5.0f, 5.0f));
      mainLightTransform->SetPosition(Vector3(-9.0f, 0.0f, -3.0f));

      Model* lanternModel = resMng->Load<Model>(TEXT("Contents/Models/Lantern/Lantern.gltf"));
      Entity* lantern = Model::Instantiate(lanternModel, world, TEXT("Lantern"));
      Entity* lanternMesh = lantern->GetChildren()[0];
      //RotateComponent* lanternRotateComponent = lantern->AddComponent<RotateComponent>();
      MeshRenderComponent* lanternRenderComponent = lanternMesh->GetComponent<MeshRenderComponent>();
      Material* lanternMaterial = lanternRenderComponent->GetMaterial();
      lanternMaterial->Save();

      Transform* lanternTransform = lantern->GetTransform();
      lanternTransform->SetScale(Vector3(0.1f, 0.1f, 0.1f));
      lanternTransform->SetPosition(Vector3(-2.5f, -0.9f, 0.0f));

      Model* damagedHelmetModel = resMng->Load<Model>(TEXT("Contents/Models/DamagedHelmet/DamagedHelmet.gltf"));
      Entity* damagedHelmet = Model::Instantiate(damagedHelmetModel, world, TEXT("DamagedHelmet"));
      Entity* damagedHelmetMesh = damagedHelmet->GetChildren()[0];
      MeshRenderComponent* helmetRenderComponent = damagedHelmetMesh->GetComponent<MeshRenderComponent>();
      Material* damagedHelmetMaterial = helmetRenderComponent->GetMaterial();
      Transform* helmetTransform = damagedHelmet->GetTransform();
      helmetTransform->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
      damagedHelmetMaterial->Save();

      Cube* cubeMesh = new Cube(renderer);
      cubeMesh->Init(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
      Entity* cube = world->CreateEntity(TEXT("Cube"));
      Transform* cubeTransform = cube->GetTransform();
      MeshRenderComponent* cubeRenderComponent = cube->AddComponent<MeshRenderComponent>();
      Material* cubeMaterial = resMng->Load<Material>(TEXT("Contents/Materials/Default.material"));
      cubeMaterial->SetScalarFactor(MaterialFactorProperty::Metallic, 0.0f);
      cubeMaterial->SetScalarFactor(MaterialFactorProperty::Roughness, 1.0f);
      cubeRenderComponent->SetMesh(cubeMesh);
      cubeRenderComponent->SetMaterial(cubeMaterial);
      cubeTransform->SetPosition(Vector3(3.5f, 0.0f, 0.0f));
      cubeTransform->SetScale(Vector3(1.0f, 1.0f, 1.0f));

      Material* planeMaterial = resMng->Load<Material>(TEXT("Contents/Materials/Metal.material"));
      Quad* quadMesh = new Quad(renderer);
      quadMesh->Init(-1.0f, -1.0f, 1.0f, 1.0f);
      Entity* plane = world->CreateEntity(TEXT("Plane"));
      Transform* planeTransform = plane->GetTransform();
      MeshRenderComponent* planeRenderComponent = plane->AddComponent<MeshRenderComponent>();
      planeRenderComponent->SetMesh(quadMesh);
      planeRenderComponent->SetMaterial(cubeMaterial);
      planeTransform->SetPosition(Vector3(0.0f, -1.0f, 0.0f));
      planeTransform->SetRotation(Quaternion(-90.0f, Vector3(1.0f, 0.0f, 0.0f)));
      planeTransform->SetScale(Vector3(30.0f, 1.0f, 30.0f));

      //world->GetComponentsFromEntities<Transform>(); // Transform은 Component를 상속하지 않기 때문에 컴파일되지 않는다.

      Texture2D* iceLakeHDR = resMng->Load<Texture2D>(TEXT("Contents/Textures/Ice_Lake/Ice_Lake_Ref.hdr"));
      renderer->SetEquirectangularMap(iceLakeHDR);

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
      execute = engine->Execute();
   }

   SafeDelete(context);
   return execute;
}