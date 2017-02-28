#include "Actor.h"
#include "ActorComponent.h"

namespace Mile
{
    void Actor::TickActor( float DeltaTime )
    {
        if ( RootComponent != nullptr )
        {
            RootComponent->Tick( DeltaTime );
        }
    }
}
