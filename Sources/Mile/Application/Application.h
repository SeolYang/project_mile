#pragma once
#include "Mile.h"

namespace Mile
{
    class MileTimer;
    class Window;
    class MILE_API Application
    {
    public:
        Application( std::unique_ptr<Window> NewTargetWindow );
        virtual ~Application( );

        virtual void Excute( ) = 0;

        static Application& GetInstance( );
        Window& GetWindow( ) const;

    private:
        std::unique_ptr<Window> MainWindow;

    private:
        static Application* Instance;

    };
}