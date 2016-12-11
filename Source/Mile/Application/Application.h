#pragma once
#include "Mile.h"

namespace Mile
{
    class Allocator;
    class Timer;
    class Application
    {
    public:
        Application( );
        virtual ~Application( );

    private:
        Timer* MainTimer;

    };
}