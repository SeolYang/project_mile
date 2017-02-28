#include "ActorComponent.h"
#include "Actor.h"

namespace Mile
{
    void ActorComponent::SetOwner( Actor* NewOwner )
    {
        DetachFromOwner( );
        if ( NewOwner != nullptr )
        {
            Owner = NewOwner;
            Owner->AttachComponent( this );
        }
    }

    void ActorComponent::DetachFromOwner( )
    {
        if ( Owner != nullptr )
        {
            Owner->DetachComponent( this );
        }
    }
}