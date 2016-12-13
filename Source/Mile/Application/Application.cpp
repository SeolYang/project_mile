#include "Application.h"
#include "Timer.h"

namespace Mile
{
    std::unique_ptr<Application> Application::AppInstance = nullptr;

    Application::Application( ) :
        AppTimer( nullptr ), bLoop( true )
    {
        AppInstance = std::make_unique<Application>( this );

        AppTimer = std::make_unique<Timer>( );
        AppTimer->Update( );
    }

    Application::~Application( )
    {
    }

    Application& Application::GetInstance( )
    {
        ASSERT_MSG( ( AppInstance != nullptr ), TEXT( "Application instance is unavaliable!" ) );
        return ( *AppInstance );
    }
}