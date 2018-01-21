#include "Core\Context.h"
#include "Core\Engine.h"
#include "Core\Logger.h"
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
#include "Math\Vector3.h"

using namespace Mile;

int main( )
{
   auto context = std::make_unique<Context>( );
   auto engine = std::make_unique<Engine>( context.get( ) );

   if ( !engine->Init( ) )
   {
      return 1;
   }

   auto world = context->GetSubSystem<World>( );
   auto resMng = context->GetSubSystem<ResourceManager>( );

   auto camera = world->CreateEntity( TEXT( "Camera" ) );
   auto camComp = camera->AddComponent<CameraComponent>( );
   auto camTransform = camera->GetTransform( );
   camTransform->SetPosition( Vector3( 0.0f, 0.0f, 0.0f ) );

   camComp->SetNearPlane( 1.0f );
   camComp->SetFarPlane( 1000.0f );
   camComp->SetFov( 70.0f );

   // Camera setup

   auto light = world->CreateEntity( TEXT( "DirectionalLight" ) );
   auto lightComp = light->AddComponent<LightComponent>( );
   auto lightTransform = light->GetTransform( );
   lightTransform->SetPosition( Vector3( 0.0f, 0.0f, 0.0f ) );
   //lightTransform->SetRotation( Quaternion(
   //   Math::DegreeToRadian( 90.0f ), Vector3::Up( ) ) );

   // Light setup

   auto model = resMng->Load<Model>( TEXT( "Contents/Models/Sphere.3ds" ) );
   auto diffuse = resMng->Load<Texture2D>( TEXT( "Contents/Textures/Pebbles_002_COLOR.jpg" ) );
   //auto diffuse = resMng->Load<Texture2D>( TEXT( "Contents/Textures/su0.png" ) );
   auto normal = resMng->Load<Texture2D>( TEXT( "Contents/Textures/Pebbles_002_NRM.jpg" ) );

   // Model setup

   std::array<Entity*, 4> modelEntities{ 
      Model::Instantiate( model, world ),
      Model::Instantiate( model, world ) ,
      Model::Instantiate( model, world ) ,
      Model::Instantiate( model, world ) };

   std::array<Transform*, 4> transforms{
      modelEntities[ 0 ]->GetTransform( ),
      modelEntities[ 1 ]->GetTransform( ),
      modelEntities[ 2 ]->GetTransform( ),
      modelEntities[ 3 ]->GetTransform( )
   };

   std::array<MeshRenderComponent*, 4> modelRenderers{
      modelEntities[ 0 ]->GetChildren( )[ 0 ]->GetComponent< MeshRenderComponent >( ),
      modelEntities[ 1 ]->GetChildren( )[ 0 ]->GetComponent< MeshRenderComponent >( ),
      modelEntities[ 2 ]->GetChildren( )[ 0 ]->GetComponent< MeshRenderComponent >( ),
      modelEntities[ 3 ]->GetChildren( )[ 0 ]->GetComponent< MeshRenderComponent >( )
   };

   std::array<Material*, 4> materials{
      modelRenderers[ 0 ]->GetMaterial( ),
      modelRenderers[ 1 ]->GetMaterial( ),
      modelRenderers[ 2 ]->GetMaterial( ),
      modelRenderers[ 3 ]->GetMaterial( )
   };

   for ( auto mat : materials )
   {
      mat->SetDiffuseMap( diffuse );
      mat->SetNormalMap( normal );
   }

   float scale = 0.04f;
   float posBegin = -150.0f;
   float posDelta = 100.0f;
   float rotBegin = -45.0f;
   float rotDelta = 22.5f;
   unsigned int t = 0;
   for ( auto transform : transforms )
   {
      transform->SetScale( Vector3( 0.5f, 0.5f, 0.5f ) );
      transform->SetRotation(
         Quaternion( Math::DegreeToRadian( rotBegin + ( rotDelta * t ) ),
         Vector3::Up( ) ), TransformSpace::World );
      transform->SetPosition(
         Vector3( posBegin + ( posDelta * t ), 0.0f, 180.0f ) );
      ++t;
   }

   MELog( context.get( ), TEXT( "TestCategory" ), ELogType::DEBUG, TEXT( "TEST MESSAGE~" ), true );

   int execute = engine->Execute( );
   return execute;
}