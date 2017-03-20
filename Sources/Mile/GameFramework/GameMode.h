#pragma once
#include "MileObject.h"
#include "MileWorld.h"

namespace Mile
{
    class MileWorld;
    class MILE_API GameMode : public Mile::Object
    {
        FORCEINLINE GameMode( const MString& NewName, MileWorld& NewWorld ) :
            World( NewWorld ),
            Object( NewName )
        {
        }

        void GameBegin( )
        {
            World.OnGameBegin( );
        }

        void GameEnd( )
        {
            World.OnGameEnd( );
        }

        FORCEINLINE MileWorld& GetWorld( ) const
        {
            return ( World );
        }

    private:
        MileWorld& World;

    };
}