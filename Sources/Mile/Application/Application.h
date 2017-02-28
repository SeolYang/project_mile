#pragma once
#include "Mile.h"

namespace Mile
{
    class Timer;
    class Application
    {
    public:
        Application( );
        virtual ~Application( );

        static Application& GetInstance( );

    private:
        static std::unique_ptr<Application> AppInstance;

    private:
        bool                   bLoop;
        std::unique_ptr<Timer> AppTimer;

    };
}