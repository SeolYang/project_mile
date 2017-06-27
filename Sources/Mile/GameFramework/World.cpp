#include "World.h"
#include "Entity.h"

namespace Mile
{
    World::~World( )
    {
        // World has responsability to delete Entities
        for ( auto Entity : Entities )
        {
            delete Entity;
        }

        Entities.Clear( );
    }

    void World::RegisterEntity( Entity* NewEntity )
    {
        if ( NewEntity != nullptr )
        {
            World* OldWorld = &( NewEntity->GetWorld( ) );
            if ( OldWorld != this )
            {
                NewEntity->SetWorld( this );
                Entities.PushBack( NewEntity );
            }
            else
            {
                if ( OldWorld != nullptr )
                {
                    OldWorld->UnRegisterEntity( NewEntity );
                }
            }
        }
    }

    void World::UnRegisterEntity( Entity* TargetEntity )
    {
        if ( TargetEntity != nullptr )
        {
            Entities.Erase( Entities.Find( TargetEntity ) );
        }
    }

    void World::UnRegisterAllEntities( )
    {
        while ( !Entities.IsEmpty( ) )
        {
            Entities[ 0 ]->SetWorld( nullptr );
            Entities.EraseAt( 0 );
        }
    }

    void World::OnBegin( )
    {
        for ( auto Entity : Entities )
        {
            Entity->OnBegin( );
        }
    }

    void World::OnEnd( )
    {
        for ( auto Entity : Entities )
        {
            Entity->OnEnd( );
        }
    }
}