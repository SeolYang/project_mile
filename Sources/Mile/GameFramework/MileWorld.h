#pragma once
#include "MileObject.h"

namespace Mile
{
    class Actor;
    class MileGameMode;
    class MILE_API MileWorld final : public MileObject
    {
    public:
        void AddActor( Actor* Actor );

    private:
        MileGameMode* GameMode;
        std::vector<Actor*> Actors;

    };
}