#pragma once
#include <chrono>
#include "Mile.h"

namespace Mile
{
    class MILE_API Timer final
    {
    public:
        Timer( );
        ~Timer( );

        void Update( );
        void Reset( );

        float GetRuntime( ) const;
        float GetDeltaTime( ) const;
        uint64 GetFrameCount( ) const;
        uint64 GetFPS( )  const;
        uint64 GetMaxFPS( ) const;
        float GetScaledDeltaTime( ) const;

        float GetTimeScale( ) const;
        void SetTimeScale( float Scale );

    private:
        uint64 TicksPerSec;

        uint64 StartupTicks;
        uint64 LastTicks;
        uint64 CurrentTicks;
        uint64 OneSecElasedTicks;

        float DeltaTime;
        uint64 FrameCount;
        uint64 FPS;
        uint64 MaxFPS;

        float TimeScale;

    };
}