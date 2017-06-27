#include "Application.h"
#include "MileWindow.h"
#include "Timer.h"

namespace Mile
{
    Application* Application::Instance = nullptr;

    Application::Application( std::unique_ptr<Window> NewTargetWindow ) :
        MainWindow( std::move( NewTargetWindow ) )
    {
    }

    Application::~Application( )
    {
    }

    Application& Application::GetInstance( )
    {
        ASSERT_MSG( ( Instance != nullptr ), TEXT( "Application instance is unavaliable!" ) );
        return ( *Instance );
    }

    Window& Application::GetWindow( ) const
    {
        return ( *MainWindow );
    }
}