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

int main( int Argc, char* Argv[ ], char* Envp[ ] )
{
    setlocale( LC_ALL, "" );
#ifdef _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    MString Str = TEXT( "Test" );
    std::cout << ( Str == MString( TEXT( "Test" ) ) ) << std::endl;

    return 0;
}
#endif