#include "MileApplication.h"
#include "Win32Window.h"
#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"
using namespace Mile;

int main( int Argc, char* Argv[], char* Envp[] )
{
    /** @todo: Replace to Application end code */
    SceneComponent* Comp1 = new SceneComponent( MString( TEXT( "SceneComp1" ) ) );
    SceneComponent* Comp2 = new SceneComponent( MString( TEXT( "SceneComp2" ) ) );
    Comp1->AttachTo( Comp2 );
    Comp2->AttachTo( Comp1 );

    Comp1->DetachFromComponent( );
    Comp2->AttachTo( Comp1 );
    return 0;
}