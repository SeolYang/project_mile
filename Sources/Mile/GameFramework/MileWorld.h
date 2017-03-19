#pragma once
#include "MileObject.h"

namespace Mile
{
    class Actor;
    class MileGameMode;
    class MILE_API MileWorld final : public Mile::Object
    {
    public:
        void AddActor( Actor* Actor );

    private:
        MileGameMode* GameMode;
        std::vector<Actor*> Actors;

    };
}