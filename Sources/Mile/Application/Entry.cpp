#include "MileApplication.h"
#include "Win32Window.h"
#include "TickManager.h"
#include "Actor.h"
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

    std::cout << "Case 1: " << std::endl;
    Actor* Act1 = new Actor(MString(TEXT("Actor1")));
    Actor* Act2 = new Actor(MString(TEXT("Actor2")));
    Act1->SetIsTick(true);
    Act2->SetIsTick(true);
    TickManager::GetInstance().Tick(0.0f);
    std::cout << std::endl;

    std::cout << "Case 2: " << std::endl;
    Act2->SetIsTick(false);
    TickManager::GetInstance().Tick(0.0f);
    std::cout << std::endl;

    std::cout << "Case 3: " << std::endl;
    Act2->SetIsTick(true);
    Act2->SetTickPriority(1);
    TickManager::GetInstance().Tick(0.0f);
    std::cout << std::endl;

    delete Act1;
    Act1 = nullptr;

    delete Act2;
    Act2 = nullptr;

    TickManager::GetInstance().DestroyInstance();

    std::cout << "Left MileObject: " << MileObject::GetObjectCount() << std::endl;
    
    /** @todo: Replace to Application end code */
    return 0;
}