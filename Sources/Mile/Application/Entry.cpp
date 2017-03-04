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

    Actor* Act = new Actor( MString( TEXT( "Owner" ) ) );
    SceneComponent* Comp1 = new SceneComponent( MString( TEXT( "Comp1" ) ) );
    SceneComponent* Comp2 = new SceneComponent( MString( TEXT( "Comp2" ) ) );
    SceneComponent* Comp3 = new SceneComponent( MString( TEXT( "Comp3" ) ) );
    Act->AttachComponent( Comp3 );
    Comp2->AttachTo( Comp1 );
    Comp1->AttachTo( Comp3 );

    std::wcout << Comp1->GetOwner( )->GetName( ).operator std::wstring( ) << std::endl;
    std::wcout << Comp2->GetOwner( )->GetName( ).operator std::wstring( ) << std::endl;
    std::wcout << Comp3->GetOwner( )->GetName( ).operator std::wstring( ) << std::endl;

    /** @todo: Replace to Application end code */
    return 0;
}