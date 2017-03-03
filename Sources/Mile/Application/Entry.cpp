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

    /** @todo: Replace to Application end code */
    SceneComponent* Comp1 = new SceneComponent( MString( TEXT( "SceneComp1" ) ) );
    SceneComponent* Comp2 = new SceneComponent( MString( TEXT( "SceneComp2" ) ) );
    Comp1->AttachTo( Comp2 );
    Comp2->AttachTo( Comp1 );

    Comp1->DetachFromComponent( );
    Comp2->AttachTo( Comp1 );

    Actor* TestActor = new Actor( MString( TEXT( "╬вем" ) ) );
    TestActor->AttachComponent( Comp1 );
    std::cout << TestActor->SetRootComponent( Comp1 ) << std::endl; 
    TestActor->DetachComponent( Comp1 );
    std::cout << TestActor->SetRootComponent( Comp1 ) << std::endl;
    return 0;
}