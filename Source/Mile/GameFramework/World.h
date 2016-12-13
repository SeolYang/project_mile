#pragma once
#include "Mile.h"

namespace Mile
{
    class Level;
    class GameMode;
    class MILE_API World final
    {

    private:
        std::unique_ptr<GameMode> CurrentMode;
        std::vector<Level*> LevelList;
        uint64 CurrentLevelIndex;

    };
}