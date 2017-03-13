#include "MileTimer.h"
#include <chrono>
using namespace std::chrono;

namespace Mile
{
    MileTimer* MileTimer::Instance = nullptr;
    MileTimer::MileTimer( ) :
        DeltaTime( 0.0f ), TimeScale( 1.0f )
    {
        QueryPerformanceFrequency( (LARGE_INTEGER*)&TicksPerSec );
        QueryPerformanceCounter( (LARGE_INTEGER*)&CurrentTicks );
        StartupTicks = CurrentTicks;
        OneSecElasedTicks = CurrentTicks;
    }

    MileTimer::~MileTimer( )
    {
    }

    MileTimer& MileTimer::GetInstance( )
    {
        if ( Instance == nullptr )
        {
            Instance = new MileTimer( );
        }

        return ( *Instance );
    }

    bool MileTimer::DestroyInstance( )
    {
        if ( Instance != nullptr )
        {
            delete Instance;
            Instance = nullptr;
            return true;
        }

        return false;
    }

    void MileTimer::Update( )
    {
        LastTicks = CurrentTicks;
        QueryPerformanceCounter( (LARGE_INTEGER*)&CurrentTicks );

        DeltaTime = static_cast<float>( LastTicks - CurrentTicks ) / TicksPerSec;

        float ElasedPerSec = static_cast<float>( CurrentTicks - OneSecElasedTicks ) / TicksPerSec;
        if ( ElasedPerSec < 1.0f )
        {
            ++FrameCount;
        }
        else
        {
            FPS = FrameCount;
            FrameCount = 0;

            if ( FPS > MaxFPS )
            {
                MaxFPS = FPS;
            }

            OneSecElasedTicks = CurrentTicks;
        }
    }

    void MileTimer::Reset( )
    {
        FPS = 0;
        MaxFPS = 0;
        FrameCount = 0;
    }

    float MileTimer::GetRuntime( ) const
    {
        return ( static_cast<float>
            ( CurrentTicks - StartupTicks ) / TicksPerSec );
    }

    float MileTimer::GetDeltaTime( ) const
    {
        return DeltaTime;
    }

    uint64 MileTimer::GetFrameCount( ) const
    {
        return FrameCount;
    }

    uint64 MileTimer::GetFPS( ) const
    {
        return FPS;
    }

    uint64 MileTimer::GetMaxFPS( ) const
    {
        return MaxFPS;
    }

    float MileTimer::GetScaledDeltaTime( ) const
    {
        return ( TimeScale * DeltaTime );
    }

    float MileTimer::GetTimeScale( ) const
    {
        return TimeScale;
    }

    void MileTimer::SetTimeScale( float Scale )
    {
        TimeScale = Scale;
    }
}