#include "Timer.h"
using namespace std::chrono;

namespace Mile
{
    Timer::TimePoint Timer::BeginApplication = Timer::TimePoint( );
    Timer::TimePoint Timer::BeforeFrame = Timer::TimePoint( );
    Timer::TimePoint Timer::AfterFrame = Timer::TimePoint( );

    float Timer::GetDeltaTime( )
    {
        return
            duration_cast<duration<float>>
            ( AfterFrame - BeforeFrame ).count( );
    }

    float Timer::GetRunningTime( )
    {
        return
            duration_cast<duration<float>>
            ( high_resolution_clock::now( ) - BeginApplication ).count( );
    }
}