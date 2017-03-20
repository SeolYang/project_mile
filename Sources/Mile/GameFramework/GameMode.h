#pragma once
#include "MileObject.h"
#include "MileWorld.h"

namespace Mile
{
    class MileWorld;
    class MILE_API GameMode : public Mile::Object
    {
        FORCEINLINE GameMode( const MString& NewName, MileWorld* NewWorld ) :
            World( NewWorld ),
            Object( NewName )
        {
        }

        void GameBegin( )
        {
            if ( World != nullptr )
            {
                World->OnGameBegin( );
            }
        }

        void GameEnd( )
        {
            if ( World != nullptr )
            {
                World->OnGameEnd( );
            }
        }

        FORCEINLINE MileWorld& GetWorld( ) const
        {
            return ( *World );
        }

        void SetWorld( MileWorld* NewWorld )
        {
            World = NewWorld;
        }

    private:
        MileWorld* World;

    };
}