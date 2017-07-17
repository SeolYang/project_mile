#include "World.h"
#include "Context.h"
#include "Entity.h"

namespace Mile
{
    World::World( Context* context ) :
        SubSystem( context ),
        m_entities( context->GetAllocator( ) )
    {

    }

    World::~World( )
    {
    }

    bool World::Initialize( )
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
        //auto newEntity = RUMIA_MAKE_SHARED( m_context->GetAllocator( ), Entity, m_context );
        auto newEntity = MakeShared<Entity>( m_context->GetAllocator( ), m_context );
        m_entities.PushBack( newEntity );

        return newEntity;
    }

    WeakEntity World::GetEntityByName( const std::string& name )
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

    Array<WeakEntity> World::GetEntities( )
    {
        Array<WeakEntity> tempEntities( m_context->GetAllocator( ) );
        for ( const auto& entity : m_entities )
        {
            tempEntities.PushBack( entity );
        }

        return std::move( tempEntities );
    }
}
