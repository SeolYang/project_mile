#include "World.h"
#include "Context.h"
#include "Entity.h"

namespace Mile
{
   World::World( Context* context ) :
      SubSystem( context )
   {

   }

   World::~World( )
   {
   }

   bool World::Init( )
   {
      return true;
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

   Entity* World::CreateEntity( )
   {
      auto newEntity = new Entity( m_context );
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
      std::vector<Entity*> tempEntities{ };
      for ( const auto& entity : m_entities )
      {
         tempEntities.push_back( entity );
      }

      return std::move( tempEntities );
   }
}
