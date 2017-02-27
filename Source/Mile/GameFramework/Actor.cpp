#include "Actor.h"

namespace Mile
{
    void Actor::TickActor( float DeltaTime )
    {
        if ( bIsTick )
        {
            for ( auto Child : Children )
            {
                Child->TickActor( DeltaTime );
            }
        }
    }
}
