#include "World.h"
#include "Entity.h"

namespace Mile
{
    World::~World( )
    {
        // World has responsibility to delete Components.
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
                if ( OldWorld != nullptr )
                {
                    OldWorld->UnRegisterEntity( NewEntity );
                }

                NewEntity->SetWorld( this );
                Entities.PushBack( NewEntity );
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