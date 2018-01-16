#include "Core\Context.h"
#include "Core\Engine.h"
#include "Core\World.h"
#include "Core\Entity.h"
#include "Component\CameraComponent.h"
#include "Component\MeshRenderComponent.h"
#include "Component\LightComponent.h"
#include "Resource\ResourceManager.h"
#include "Resource\Model.h"
#include "Resource\ModelLoader.h"
#include "Resource\Material.h"
#include "Resource\Texture2D.h"

using namespace Mile;

int main( )
{
   auto context = std::make_unique<Context>( );
   auto engine = new Engine( context.get( ) );

   if ( !engine->Init( ) )
   {
      return 1;
   }

   auto world = context->GetSubSystem<World>( );
   auto resMng = context->GetSubSystem<ResourceManager>( );

   auto camera = world->CreateEntity( TEXT( "Camera" ) );
   auto camComp = camera->AddComponent<CameraComponent>( );
   auto camTransform = camera->GetTransform( );
   camTransform->SetPosition( Vector3( 0.0f, 0.0f, -100.0f ) );

   // Camera setup

   auto light = world->CreateEntity( TEXT( "DirectionalLight" ) );
   auto lightComp = camera->AddComponent<LightComponent>( );

   // Light setup

   auto model = resMng->Load<Model>( TEXT( "Contents/Models/dragon.obj" ) );
   auto modelEntity = Model::Instantiate( model, world );
   auto modelMeshRenderer = modelEntity->GetComponent<MeshRenderComponent>( );
   auto mat = modelMeshRenderer->GetMaterial( );

   auto texture = resMng->Load<Texture2D>( TEXT( "Contents/Textures/test.png" ) );
   mat->SetDiffuseMap( texture );

   // Model setup

   int execute = engine->Execute( );
   SafeDelete( engine );
   return execute;
}