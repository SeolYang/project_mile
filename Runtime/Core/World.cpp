#include "World.h"
#include "Context.h"
#include "Entity.h"
#include "Resource\ResourceManager.h"
#include "Config.h"
#include "Resource\PlainText.h"
#include <iostream>

namespace Mile
{
   World::World( Context* context ) :
      SubSystem( context )
   {

   }

   World::~World( )
   {
   }

   std::string World::Serialize( ) const
   {
      std::string res = "{ \"Entities\": [  ";
      /* Entity serialize */
      for ( auto entity : m_entities )
      {
         res += entity->Serialize( );
         res += ", ";
      }
      res[ res.length( ) - 2 ] = ']';
      res += "}";
      return res;
   }

   void World::DeSerialize( const json& jsonData )
   {
      std::vector<json> entities = jsonData[ "Entities" ];
      for ( auto entity : entities )
      {
         Entity* temp = CreateEntity( TEXT( "" ) );
         temp->DeSerialize( entity );
      }
   }

   bool World::Init( )
   {
      auto configSys = m_context->GetSubSystem<ConfigSystem>( );
      auto engineConfig = configSys->GetConfig( TEXT( "Engine" ) );
      String defaultPath = String2WString(engineConfig.second[ "World" ]);

      return LoadFrom( defaultPath );
   }

   void World::DeInit( )
   {
      //SaveTo( TEXT( "Contents/Worlds/Test.json" ) );
   }

   void World::Start( )
   {
      for ( auto entity : m_entities )
      {
         entity->Start( );
      }
   }

   void World::Update( )
   {
      for ( auto entity : m_entities )
      {
         entity->Update( );
      }
   }

   Entity* World::CreateEntity( const String& name )
   {
      auto newEntity = new Entity( this, name );
      m_entities.push_back( newEntity );

      return newEntity;
   }

   Entity* World::GetEntityByName( const String& name )
   {
      for ( const auto& entity : m_entities )
      {
         if ( name == ( entity )->GetName( ) )
         {
            return entity;
         }
      }

      return nullptr;
   }

   std::vector<Entity*> World::GetEntities( )
   {
      return m_entities;
   }

   std::vector<Entity*> World::GetRootEntities( )
   {
      std::vector<Entity*> rootEntities{ };
      for ( auto entity : m_entities )
      {
         if ( entity->GetParent( ) == nullptr )
         {
            rootEntities.push_back( entity );
         }
      }

      return rootEntities;
   }

   bool World::LoadFrom( const String& filePath )
   {
      auto resMng = m_context->GetSubSystem<ResourceManager>( );
      auto res = resMng->Create<PlainText<std::string>>( filePath );
      if ( res.expired( ) )
      {
         return false;
      }

      this->m_loadedData = res;
      this->DeSerialize( json::parse( res.lock( )->GetData( ) ) );
      return true;
   }

   bool World::SaveTo( const String& filePath )
   {
      auto resMng = m_context->GetSubSystem<ResourceManager>( );
      auto res = resMng->GetByPath<PlainText<std::string>>( filePath );
      if ( res.expired( ) )
      {
         res = resMng->Create<PlainText<std::string>>( filePath );
         if ( res.expired( ) )
         {
            return false;
         }
      }

      auto resPtr = res.lock( );
      resPtr->SetData( this->Serialize( ) );
      return resPtr->Save( );
   }

   bool World::Save( )
   {
      if ( m_loadedData.expired( ) )
      {
         return false;
      }

      auto rawPtr = m_loadedData._Get( );
      rawPtr->SetData( this->Serialize( ) );
      return rawPtr->Save( );
   }
}
