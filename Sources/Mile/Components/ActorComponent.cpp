#include "ActorComponent.h"
#include "Actor.h"

namespace Mile
{
    void ActorComponent::SetOwner( Actor* NewOwner )
    {
        DetachFromOwner( );
        Owner = NewOwner;
    }

    void ActorComponent::DetachFromOwner( )
    {
        if ( Owner != nullptr )
        {
            Owner->DetachComponent( this );
        }
    }
}