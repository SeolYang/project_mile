#include "Core\Context.h"
#include "Core\Engine.h"
#include "Core\Logger.h"
#include "GameFramework\World.h"
#include "GameFramework\Entity.h"
#include "Component\CameraComponent.h"
#include "Component\MeshRenderComponent.h"
#include "Component\LightComponent.h"
#include "Resource\ResourceManager.h"
#include "Resource\Model.h"
#include "Resource\ModelLoader.h"
#include "Resource\Material.h"
#include "Resource\Texture2D.h"
#include "Math\Vector3.h"
#include "MT\ThreadPool.h"
#include "RotateComponent.h"

using namespace Mile;

int main( )
{
   auto context = new Context( );
   auto engine = new Engine( context );

   if ( !engine->Init( ) )
   {
      return 1;
   }

   auto world = context->GetSubSystem<World>( );
   auto resMng = context->GetSubSystem<ResourceManager>( );

   //Entity* camera = world->CreateEntity( TEXT( "Camera" ) );
   //CameraComponent* camComponent = camera->AddComponent<CameraComponent>( );
   //Transform* camTransform = camera->GetTransform( );
   //camComponent->SetNearPlane(1.0f);
   //camComponent->SetFarPlane(1000.0f);
   //camComponent->SetFov(60.0f);
   //camTransform->SetPosition(Vector3(0.0f, 0.0f, -5.0f));

   //Entity* mainLight = world->CreateEntity(TEXT("Upper Light"));
   //LightComponent* mainLightComponent = mainLight->AddComponent<LightComponent>();
   //Transform* mainLightTransform = mainLight->GetTransform();
   //mainLightComponent->SetLightType(ELightType::Point);
   //mainLightComponent->SetRadiance(Vector3(200.0f, 200.0f, 200.0f));
   //mainLightTransform->SetPosition(Vector3(0.0f, 0.0f, -5.0f));

   //Model* damagedHelmetModel = resMng->Load<Model>(TEXT("Contents/Models/DamagedHelmet/DamagedHelmet.gltf"));
   //Entity* damagedHelmet = Model::Instantiate(damagedHelmetModel, world, TEXT("DamagedHelmet"));
   //Entity* damagedHelmetMesh = damagedHelmet->GetChildren()[0];
   //RotateComponent* damagedHelmetRotation = damagedHelmet->AddComponent<RotateComponent>();
   //MeshRenderComponent* helmetRenderComponent = damagedHelmetMesh->GetComponent<MeshRenderComponent>();
   //Material* damagedHelmetMaterial = helmetRenderComponent->GetMaterial();
   //damagedHelmetMaterial->Save();

   //Model* lanternModel = resMng->Load<Model>(TEXT("Contents/Models/Lantern/Lantern.gltf"));
   //Entity* lantern = Model::Instantiate(lanternModel, world, TEXT("Lantern"));
   //Entity* lanternMesh = lantern->GetChildren()[0];
   //RotateComponent* lanternRotateComponent = lantern->AddComponent<RotateComponent>();
   //MeshRenderComponent* lanternRenderComponent = lanternMesh->GetComponent<MeshRenderComponent>();
   //Material* lanternMaterial = lanternRenderComponent->GetMaterial();
   //lanternMaterial->Save();

   //Transform* lanternTransform = lantern->GetTransform();
   //lanternTransform->SetScale(Vector3(0.1f, 0.1f, 0.1f));
   //lanternTransform->SetPosition(Vector3(-2.5f, -0.9f, 0.0f));

   //Model* metalRoughSpheresModel = resMng->Load<Model>(TEXT("Contents/Models/MetalRoughSpheres/MetalRoughSpheres.gltf"));
   //Entity* spheresEntity = Model::Instantiate(metalRoughSpheresModel, world, TEXT("Spheres"));
   //Entity* spheresMesh = spheresEntity->GetChildren()[0];
   //MeshRenderComponent* spheresRenderComponent = spheresMesh->GetComponent<MeshRenderComponent>();
   //Material* spheresMaterial = spheresRenderComponent->GetMaterial();
   //spheresMaterial->Save();

   //Transform* spheresTransform = spheresEntity->GetTransform();
   ////spheresTransform->SetScale(Vector3(0.1f, 0.1f, 0.1f));
   //spheresTransform->SetPosition(Vector3(0.0f, 0.0f, 4.0f));

   int execute = engine->Execute( );
   world->Save();
   SafeDelete( context );
   return execute;
}