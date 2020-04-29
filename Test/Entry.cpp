#include "Core/Context.h"
#include "Core/Engine.h"
#include "Core/Logger.h"
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
#include "Rendering/RendererDX11.h"
#include "Math/Vector3.h"
#include "MT/ThreadPool.h"
#include "RotateComponent.h"

using namespace Mile;

int main( )
{
   auto context = new Context();
   auto engine = new Engine(context);
   int execute = 1;

   if (engine->Init())
   {
      auto world = Engine::GetWorld();
      auto resMng = Engine::GetResourceManager();
      auto renderer = Engine::GetRenderer();

      Cube* cubeMesh = new Cube(renderer);
      cubeMesh->Init(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
      Entity* cube = world->CreateEntity(TEXT("Cube"));
      Transform* cubeTransform = cube->GetTransform();
      MeshRenderComponent* cubeRenderComponent = cube->AddComponent<MeshRenderComponent>();
      RotateComponent* cubeRotation = cube->AddComponent<RotateComponent>();
      Material* cubeMaterial = resMng->Load<Material>(TEXT("Contents/Materials/Default.material"));
      cubeRenderComponent->SetMesh(cubeMesh);
      cubeRenderComponent->SetMaterial(cubeMaterial);
      cubeTransform->SetPosition(Vector3(3.5f, 0.0f, 1.5f));
      cubeTransform->SetScale(Vector3(1.0f, 1.0f, 1.0f));

      Entity* camera = world->CreateEntity(TEXT("Camera"));
      CameraComponent* camComponent = camera->AddComponent<CameraComponent>();
      Transform* camTransform = camera->GetTransform();
      camComponent->SetNearPlane(0.1f);
      camComponent->SetFarPlane(1000.0f);
      camComponent->SetFov(90.0f);
      camTransform->SetPosition(Vector3(0.0f, 0.0f, -5.0f));

      Entity* mainLight = world->CreateEntity(TEXT("Main Light"));
      LightComponent* mainLightComponent = mainLight->AddComponent<LightComponent>();
      Transform* mainLightTransform = mainLight->GetTransform();
      mainLightComponent->SetLightType(ELightType::Point);
      mainLightComponent->SetRadiance(Vector3(200.0f, 200.0f, 200.0f));
      mainLightTransform->SetPosition(Vector3(-9.0f, 0.0f, -3.0f));

      Entity* secondLight = world->CreateEntity(TEXT("Second Light"));
      LightComponent* secondLightComponent = secondLight->AddComponent<LightComponent>();
      Transform* secondLightTransform = secondLight->GetTransform();
      secondLightComponent->SetLightType(ELightType::Point);
      secondLightComponent->SetRadiance(Vector3(400.0f, 400.0f, 400.0f));
      secondLightTransform->SetPosition(Vector3(0.0f, 7.0f, -3.0f));

      Model* damagedHelmetModel = resMng->Load<Model>(TEXT("Contents/Models/DamagedHelmet/DamagedHelmet.gltf"));
      Entity* damagedHelmet = Model::Instantiate(damagedHelmetModel, world, TEXT("DamagedHelmet"));
      Entity* damagedHelmetMesh = damagedHelmet->GetChildren()[0];
      RotateComponent* damagedHelmetRotation = damagedHelmet->AddComponent<RotateComponent>();
      MeshRenderComponent* helmetRenderComponent = damagedHelmetMesh->GetComponent<MeshRenderComponent>();
      Material* damagedHelmetMaterial = helmetRenderComponent->GetMaterial();
      Transform* helmetTransform = damagedHelmet->GetTransform();
      helmetTransform->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
      damagedHelmetMaterial->Save();

      Model* lanternModel = resMng->Load<Model>(TEXT("Contents/Models/Lantern/Lantern.gltf"));
      Entity* lantern = Model::Instantiate(lanternModel, world, TEXT("Lantern"));
      Entity* lanternMesh = lantern->GetChildren()[0];
      RotateComponent* lanternRotateComponent = lantern->AddComponent<RotateComponent>();
      MeshRenderComponent* lanternRenderComponent = lanternMesh->GetComponent<MeshRenderComponent>();
      Material* lanternMaterial = lanternRenderComponent->GetMaterial();
      lanternMaterial->Save();

      Transform* lanternTransform = lantern->GetTransform();
      lanternTransform->SetScale(Vector3(0.1f, 0.1f, 0.1f));
      lanternTransform->SetPosition(Vector3(-2.5f, -0.9f, 0.0f));

      Model* metalRoughSpheresModel = resMng->Load<Model>(TEXT("Contents/Models/MetalRoughSpheres/MetalRoughSpheres.gltf"));
      Entity* spheresEntity = Model::Instantiate(metalRoughSpheresModel, world, TEXT("Spheres"));
      Entity* spheresMesh = spheresEntity->GetChildren()[0];
      MeshRenderComponent* spheresRenderComponent = spheresMesh->GetComponent<MeshRenderComponent>();
      Material* spheresMaterial = spheresRenderComponent->GetMaterial();
      spheresMaterial->Save();

      Transform* spheresTransform = spheresEntity->GetTransform();
      spheresTransform->SetPosition(Vector3(0.0f, 0.0f, 4.0f));

      //world->GetComponentsFromEntities<Transform>(); // Transform은 Component를 상속하지 않기 때문에 컴파일되지 않는다.

      Texture2D* equirectangularMap = resMng->Load<Texture2D>(TEXT("Contents/Textures/Ice_Lake/Ice_Lake_Ref.hdr"));
      //Texture2D* equirectangularMap = resMng->Load<Texture2D>(TEXT("Contents/Textures/Newport_Loft/Newport_Loft_8k.jpg"));
      //Texture2D* equirectangularMap = resMng->Load<Texture2D>(TEXT("Contents/Textures/Ice_Lake/Ice_Lake_HiRes_TMap.jpg"));
      //Texture2D* equirectangularMap = resMng->Load<Texture2D>(TEXT("Contents/Textures/Milkyway/Milkyway_BG.jpg"));
      //Texture2D* equirectangularMap = resMng->Load<Texture2D>(TEXT("Contents/Textures/Winter_Forest/WinterForest_8k.jpg"));
      //Texture2D* equirectangularMap = resMng->Load<Texture2D>(TEXT("Contents/Textures/Theatre_Center/Theatre-Center_8k_TMap.jpg"));
      renderer->SetEquirectangularMap(equirectangularMap);
      renderer->SetComputeIBLAsRealtime(false);
      renderer->SetAmbientOcclusionFactor(0.7f);
      renderer->SetGammaFactor(2.2f);
      renderer->SetToneMappingFactor(0.3f);
      execute = engine->Execute();
   }

   SafeDelete(context);
   return execute;
}