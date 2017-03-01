#include "ActorComponent.h"
#include "Actor.h"

namespace Mile
{
    void ActorComponent::SetOwner( Actor* Owner )
    {
        if ( OwnerPrivate != nullptr )
        {
            OwnerPrivate->RemoveComponent( this );
        }

        OwnerPrivate = Owner;
    }
}