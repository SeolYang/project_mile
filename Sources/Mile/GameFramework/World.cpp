#include "World.h"
#include "Actor.h"

namespace Mile
{
    void World::AddActor( Actor* Actor )
    {
        Actors.PushBack( Actor );
    }

    void World::OnGameBegin( )
    {
        for ( auto Actor : Actors )
        {
            Actor->OnGameBegin( );
        }
    }

    void World::OnGameEnd( )
    {
        for ( auto Actor : Actors )
        {
            Actor->OnGameEnd( );
        }
    }
}