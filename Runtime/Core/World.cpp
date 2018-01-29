#include "World.h"
#include "Context.h"
#include "Entity.h"
#include "Logger.h"
#include "Config.h"
#include "Resource\ResourceManager.h"
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
      DeInit( );
   }

   std::string World::Serialize( ) const
   {
      std::string res = "{ \"Entities\": [  ";
      /* Entity serialize */
      for ( auto entity : m_entities )
      {
         if ( !entity->HasParent( ) )
         {
            res += entity->Serialize( );
            res += ", ";
         }
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
      if ( m_context == nullptr || m_bIsInitialized )
      {
         MELog( m_context, TEXT( "World" ), ELogType::WARNING, TEXT( "World already initialized." ), true );
         return false;
      }

      auto configSys = m_context->GetSubSystem<ConfigSystem>( );
      auto engineConfig = configSys->GetConfig( TEXT( "Engine" ) );
      String defaultPath = String2WString(engineConfig.second[ "World" ]);

      auto loaded = LoadFrom( defaultPath );
      if ( !loaded )
      {
         MELog( m_context, TEXT( "World" ), ELogType::FATAL, TEXT( "Failed to load world from " ) + defaultPath, true );
         return false;
      }

      MELog( m_context, TEXT( "World" ), ELogType::MESSAGE, TEXT( "World Initialized!" ), true );
      m_bIsInitialized = true;
      return true;
   }

   void World::DeInit( )
   {
      if ( m_bIsInitialized )
      {
         //SaveTo( TEXT( "Contents/Worlds/Test.json" ) );
         SubSystem::DeInit( );
         MELog( m_context, TEXT( "World" ), ELogType::MESSAGE, TEXT( "World deinitialized." ), true );
      }
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
      if ( res != nullptr )
      {
         this->m_loadedData = res;
         this->DeSerialize( json::parse( res->GetData( ) ) );
         MELog( m_context, TEXT( "World" ), ELogType::MESSAGE, TEXT( "World loaded. : " ) + filePath, true );
         return true;
      }

      MELog( m_context, TEXT( "World" ), ELogType::FATAL, TEXT( "World load failed. : " ) + filePath, true );
      return false;
   }

   bool World::SaveTo( const String& filePath )
   {
      auto resMng = m_context->GetSubSystem<ResourceManager>( );
      auto res = resMng->GetByPath<PlainText<std::string>>( filePath );

      if ( res == nullptr )
      {
         res = resMng->Create<PlainText<std::string>>( filePath );
      }

      if ( res != nullptr )
      {
         res->SetData( this->Serialize( ) );
         return res->Save( );
      }

      return false;
   }

   bool World::Save( )
   {
      if ( m_loadedData != nullptr )
      {
         m_loadedData->SetData( this->Serialize( ) );
         return m_loadedData->Save( );
      }

      return false;
   }
}
