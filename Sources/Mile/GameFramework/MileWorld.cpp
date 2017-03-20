#include "MileWorld.h"
#include "Actor.h"

namespace Mile
{
    void MileWorld::AddActor( Actor* Actor )
    {
        Actors.push_back( Actor );
    }

    void MileWorld::OnGameBegin( )
    {
        for ( auto Actor : Actors )
        {
            Actor->OnGameBegin( );
        }
    }

    void MileWorld::OnGameEnd( )
    {
        for ( auto Actor : Actors )
        {
            Actor->OnGameEnd( );
        }
    }
}
