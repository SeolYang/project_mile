#include "ActorComponent.h"
#include "Actor.h"

namespace Mile
{
    void ActorComponent::SetOwner( Actor* Owner, bool bIsDetachBeforeSetNewOwner )
    {
        if ( ( OwnerPrivate != nullptr ) && bIsDetachBeforeSetNewOwner )
        {
            OwnerPrivate->DetachComponent( this );
        }

        OwnerPrivate = Owner;
    }

    void ActorComponent::SetOwnerRecursively( Actor* Owner, bool bIsDetachBeforeSetNewOwner )
    {
        SetOwner( Owner, bIsDetachBeforeSetNewOwner );
    }
}