#include "Timer.h"
using namespace std::chrono;

namespace Mile
{
    Timer::Timer( ) :
        DeltaTime( 0.0f ), TimeScale( 1.0f )
    {
        QueryPerformanceFrequency( (LARGE_INTEGER*)&TicksPerSec );
        QueryPerformanceCounter( (LARGE_INTEGER*)&CurrentTicks );
        StartupTicks = CurrentTicks;
        OneSecElasedTicks = CurrentTicks;
    }

    Timer::~Timer( )
    {
    }

    void Timer::Update( )
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

    void Timer::Reset( )
    {
        FPS = 0;
        MaxFPS = 0;
        FrameCount = 0;
    }

    float Timer::GetRuntime( ) const
    {
        return ( static_cast<float>
            ( CurrentTicks - StartupTicks ) / TicksPerSec );
    }

    float Timer::GetDeltaTime( ) const
    {
        return DeltaTime;
    }

    uint64 Timer::GetFrameCount( ) const
    {
        return FrameCount;
    }

    uint64 Timer::GetFPS( ) const
    {
        return FPS;
    }

    uint64 Timer::GetMaxFPS( ) const
    {
        return MaxFPS;
    }

    float Timer::GetScaledDeltaTime( ) const
    {
        return ( TimeScale * DeltaTime );
    }

    float Timer::GetTimeScale( ) const
    {
        return TimeScale;
    }

    void Timer::SetTimeScale( float Scale )
    {
        TimeScale = Scale;
    }
}