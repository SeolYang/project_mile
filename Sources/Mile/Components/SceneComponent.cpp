#include "SceneComponent.h"

namespace Mile
{
    bool SceneComponent::AttachTo( SceneComponent* NewParent )
    {
        bool bIsValidAttachment =
            NewParent != nullptr && ParentPrivate != NewParent && NewParent->GetParent( ) != this;
        if ( bIsValidAttachment )
        {
            DetachFromComponent( );
            ParentPrivate = NewParent;
            ParentPrivate->AddAttachedComponent( this );
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
            if ( ParentPrivate->RemoveAttachedComponent( this ) )
            {
                ParentPrivate = nullptr;
            }
        }
    }

    void SceneComponent::AddAttachedComponent( SceneComponent* Component )
    {
        Components.push_back( Component );
    }

    bool SceneComponent::RemoveAttachedComponent( SceneComponent* Component )
    {
        for ( auto FoundComponent = Components.begin( );
            ( *FoundComponent ) != this;
            ++FoundComponent )
        {
            Components.erase( FoundComponent );
            return true;
        }
    }

}