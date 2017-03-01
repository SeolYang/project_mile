#include "SceneComponent.h"

namespace Mile
{
    bool SceneComponent::AttachTo( SceneComponent* NewParent )
    {
        bool bIsValidAttachment =
            NewParent != nullptr && ParentPrivate != NewParent;
        if ( bIsValidAttachment )
        {
            DetachFromComponent( );
            ParentPrivate = NewParent;
            ParentPrivate->GetAttachedComponents( ).push_back( this );
            return true;
        }

        return false;
    }

    void SceneComponent::DetachFromComponent( )
    {
        bool bIsValidDetachment =
            ParentPrivate != nullptr;
        if ( bIsValidDetachment )
        {
            for ( auto FoundComponent = ParentPrivate->GetAttachedComponents( ).begin( );
                ( *FoundComponent ) != this;
                ++FoundComponent )
            {
                ParentPrivate->GetAttachedComponents( ).erase( FoundComponent );
                ParentPrivate = nullptr;
                break;
            }
        }
    }
}