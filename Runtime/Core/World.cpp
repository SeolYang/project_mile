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
      return false;
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

   WeakEntity World::CreateEntity( )
   {
      auto newEntity = std::make_shared<Entity>( m_context );
      m_entities.push_back( newEntity );

      return newEntity;
   }

   WeakEntity World::GetEntityByName( const String& name )
   {
      for ( const auto& entity : m_entities )
      {
         if ( name == ( entity )->GetName( ) )
         {
            return entity;
         }
      }

      return WeakEntity( );
   }

   std::vector<WeakEntity> World::GetEntities( )
   {
      std::vector<WeakEntity> tempEntities{ };
      for ( const auto& entity : m_entities )
      {
         tempEntities.push_back( entity );
      }

      return std::move( tempEntities );
   }
}
