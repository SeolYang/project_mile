#pragma once
#include "Mile.h"
#include "Win32Window.h"

namespace Mile
{
    class MileTimer;
    class Window;
    class MILE_API Application
    {
    public:
        Application( std::unique_ptr<Window> NewTargetWindow );
        virtual ~Application( );

        virtual void Initialize( ) = 0;
        virtual void Running( ) = 0;
        virtual void DeInitialize( ) = 0;

        virtual void Excute( )
        {
            Initialize( );
            Running( );
            DeInitialize( );
        }

        static Application& GetInstance( );
        Window& GetWindow( ) const;

    private:
        std::unique_ptr<Window> MainWindow;

    private:
        static Application* Instance;

    };
}