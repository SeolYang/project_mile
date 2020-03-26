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

   Entity* camera = world->CreateEntity( TEXT( "Camera" ) );
   CameraComponent* camComponent = camera->AddComponent<CameraComponent>( );
   Transform* camTransform = camera->GetTransform( );
   camComponent->SetNearPlane(1.0f);
   camComponent->SetFarPlane(1000.0f);
   camComponent->SetFov(60.0f);
   camTransform->SetPosition(Vector3(0.0f, 0.0f, -5.0f));

   Entity* upperLight = world->CreateEntity(TEXT("Upper Light"));
   LightComponent* upperLightComponent = upperLight->AddComponent<LightComponent>();
   Transform* upperLightTransform = upperLight->GetTransform();
   upperLightComponent->SetLightType(ELightType::Point);
   upperLightComponent->SetRadiance(Vector3(100.0f, 100.0f, 100.0f));
   upperLightTransform->SetPosition(Vector3(0.0f, 10.0f, 0.0f));

   Entity* leftLight = world->CreateEntity(TEXT("Left Light"));
   LightComponent* leftLightComponent = leftLight->AddComponent<LightComponent>();
   Transform* leftLightTransform = leftLight->GetTransform();
   leftLightComponent->SetLightType(ELightType::Point);
   leftLightComponent->SetRadiance(Vector3(0.0f, 50.0f, 0.0f));
   leftLightTransform->SetPosition(Vector3(-15.0f, 0.0f, 0.0f));

   Entity* rightLight = world->CreateEntity(TEXT("Right Light"));
   LightComponent* rightLightComponent = rightLight->AddComponent<LightComponent>();
   Transform* rightLightTransform = rightLight->GetTransform();
   rightLightComponent->SetLightType(ELightType::Point);
   rightLightComponent->SetRadiance(Vector3(0.0f, 0.0f, 50.0f));
   rightLightTransform->SetPosition(Vector3(15.0f, 0.0f, 0.0f));

   Model* damagedHelmetModel = resMng->Load<Model>(TEXT("Contents/Models/DamagedHelmet/DamagedHelmet.gltf"));
   Entity* damagedHelmet = Model::Instantiate(damagedHelmetModel, world, TEXT("DamagedHelmet"));
   Entity* damagedHelmetMesh = damagedHelmet->GetChildren()[0];
   RotateComponent* damagedHelmetRotation = damagedHelmet->AddComponent<RotateComponent>();
   MeshRenderComponent* renderComponent = damagedHelmetMesh->GetComponent<MeshRenderComponent>();
   Material* damagedHelmetMaterial = renderComponent->GetMaterial();
   damagedHelmetMaterial->Save();

   int execute = engine->Execute( );
   //world->Save();
   SafeDelete( context );
   return execute;
}