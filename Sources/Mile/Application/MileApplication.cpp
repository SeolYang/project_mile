#include "MileApplication.h"
#include "MileWindow.h"
#include "MileTimer.h"

namespace Mile
{
    MileApplication* MileApplication::Instance = nullptr;

    MileApplication::MileApplication( std::unique_ptr<MileWindow> NewTargetWindow ) :
        Window( std::move( NewTargetWindow ) )
    {
        Timer = std::make_unique<MileTimer>( );
        Timer->Update( );
    }

    MileApplication::~MileApplication( )
    {
    }

    MileApplication& MileApplication::GetInstance( )
    {
        ASSERT_MSG( ( Instance == nullptr ), TEXT( "Application instance is unavaliable!" ) );
        return ( *Instance );
    }
}