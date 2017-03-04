#include "MileApplication.h"
#include "Win32Window.h"
#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"
using namespace Mile;

#ifdef _DEBUG
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif

int main( int Argc, char* Argv[], char* Envp[] )
{
#ifdef _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    setlocale( LC_ALL, "" );

    SceneComponent Comp1{ MString( TEXT( "Blah" ) ) };
    SceneComponent Comp2{ std::move( Comp1 ) };
    SceneComponent Comp3{ MString( TEXT( "Blah2" ) ) };
    Comp3.AttachTo( &Comp1 );
    Comp3.AttachTo( &Comp2 );
    /** @todo: Replace to Application end code */
    return 0;
}