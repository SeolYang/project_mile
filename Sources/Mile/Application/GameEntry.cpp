#ifdef MILE_GAME
#include "Application.h"
#include "Win32Window.h"
#include "TickManager.h"
#include "MileInputSystem.h"
#include "Actor.h"
using namespace Mile;

#ifdef _DEBUG
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif

void Test( )
{
}

int main( int Argc, char* Argv[ ], char* Envp[ ] )
{
#ifdef _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    setlocale( LC_ALL, "" );

    MString TestStr = MString( TEXT( "Act" ) );
    InputSystem::Instance( ).MapAction( TestStr, SActionMappingProperty( EInputKey::IK_None ) );
    InputSystem::Instance( ).BindAction( MString( TEXT( "Act" ) ), EActionInputEvent::IE_Any, Test );
    std::cout << InputSystem::Instance( ).IsMappedAction( TestStr ) << std::endl;
    std::cout << InputSystem::Instance( ).IsBindedAction( MString( TEXT( "Act" ) ), EActionInputEvent::IE_Any ) << std::endl;

    InputSystem::Instance( ).UnbindAction( MString( TEXT( "Act" ) ), EActionInputEvent::IE_Pressed );
    std::cout << InputSystem::Instance( ).IsBindedAction( MString( TEXT( "Act" ) ), EActionInputEvent::IE_Any ) << std::endl;
    std::cout << InputSystem::Instance( ).IsBindedAction( MString( TEXT( "Act" ) ), EActionInputEvent::IE_Pressed ) << std::endl;
    std::cout << InputSystem::Instance( ).IsBindedAction( MString( TEXT( "Act" ) ), EActionInputEvent::IE_Released ) << std::endl;

    InputSystem::Instance( ).BindAction( TestStr, EActionInputEvent::IE_Pressed, Test );
    std::cout << InputSystem::Instance( ).IsBindedAction( MString( TEXT( "Act" ) ), EActionInputEvent::IE_Any ) << std::endl;

    InputSystem::Instance( ).UnmapAction( TestStr );
    std::cout << InputSystem::Instance( ).IsMappedAction( TestStr ) << std::endl;
    std::cout << InputSystem::Instance( ).IsBindedAction( MString( TEXT( "Act" ) ), EActionInputEvent::IE_Released ) << std::endl;

    InputSystem::DestroyInstance( );
    return 0;
}
#endif