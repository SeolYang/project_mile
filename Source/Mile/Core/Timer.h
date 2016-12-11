#pragma once
#include <chrono>
#include "Mile.h"

namespace Mile
{
    class MILE_API Timer final
    {
    public:
        static float GetDeltaTime( );
        static float GetRunningTime( );

    private:
        static void OnBegin( )
        {
            BeginApplication = high_resolution_clock::now( );
        }

        static void PreUpdate( )
        {
            BeforeFrame = high_resolution_clock::now( );
        }

        static void PostUpdate( )
        {
            AfterFrame = high_resolution_clock::now( );
        }

    private:
        using TimePoint = std::chrono::high_resolution_clock::time_point;
        static TimePoint BeginApplication;

        static TimePoint BeforeFrame;
        static TimePoint AfterFrame;

    };
}