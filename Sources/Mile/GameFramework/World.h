#pragma once
#include "MileObject.h"
#include "Rumia/Array.h"

namespace Mile
{
    class Actor;
    class MILE_API World final : public Mile::Object
    {
    public:
        void AddActor( Actor* Actor );

        void OnGameBegin( );
        void OnGameEnd( );

    private:
        Rumia::Array<Actor*> Actors;

    };
}