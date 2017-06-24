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

            auto ParentOwner = ParentPrivate->GetOwner( );
            auto Owner = this->GetOwner( );
            if ( ParentOwner != Owner )
            {
                SetOwnerRecursively( ParentOwner );
            }
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
                SetOwnerRecursively( nullptr );
            }
        }
    }

    void SceneComponent::AddAttachedComponent( SceneComponent* Component )
    {
        Children.PushBack( Component );
    }

    bool SceneComponent::RemoveAttachedComponent( SceneComponent* Component )
    {
        for ( auto FoundComponent = Children.begin( );
              FoundComponent != Children.end( );
              ++FoundComponent )
        {
            if ( ( *FoundComponent ) == Component )
            {
                Children.Erase( FoundComponent );
                return true;
            }
        }

        return false;
    }

    void SceneComponent::SetOwnerRecursively( Actor* NewOwner, bool bIsDetachBeforeSetNewOwner )
    {
        this->SetOwner( NewOwner, bIsDetachBeforeSetNewOwner );
        for ( auto Child = Children.begin( );
              Child != Children.end( );
              ++Child )
        {
            ( *Child )->SetOwnerRecursively( NewOwner, bIsDetachBeforeSetNewOwner );
        }
    }

}