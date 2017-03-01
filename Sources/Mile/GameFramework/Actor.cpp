#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"

namespace Mile
{
    void Actor::AddComponent( ActorComponent* Component )
    {
        bool bIsValidComponent =
            Component != nullptr && Component->GetOwner( ) != this;
        if ( bIsValidComponent )
        {
            Component->SetOwner( this );
            Components.push_back( Component );
        }
    }

    void Actor::RemoveComponent( ActorComponent* Component )
    {
        bool bIsValidComponent = 
            Component != nullptr && Component->GetOwner( ) == this;
        if ( bIsValidComponent )
        {
            for ( auto FoundComponent = Components.begin( );
                ( *FoundComponent ) != Component; ++FoundComponent )
            {
                ( *FoundComponent )->SetOwner( nullptr );
                Components.erase( FoundComponent );
                break;
            }
        }
    }

    bool Actor::AttachToComponent( SceneComponent* Parent )
    {
        bool bIsValidAttachment =
            Parent != nullptr && RootComponent != nullptr;
        if ( bIsValidAttachment )
        {
            AttachToActor( Parent->GetOwner( ) );
            return true;
        }

        return false;
    }

    void Actor::DetachFromComponent( )
    {
        bool bIsValidDetachment =
            RootComponent != nullptr && RootComponent->GetParent( ) != nullptr;
        if ( bIsValidDetachment )
        {
            DetachFromParent( );
        }
    }

    void Actor::AttachToActor( Actor* Parent )
    {
        ParentPrivate = Parent;
    }

    void Actor::DetachFromParent( )
    {
        ParentPrivate = nullptr;
    }

    bool Actor::SetRootComponent( SceneComponent* NewRootComponent )
    {
        bool bIsValidComponent =
            NewRootComponent != nullptr && NewRootComponent->GetOwner( ) == this;
        if ( bIsValidComponent )
        {
            RootComponent = NewRootComponent;
            return true;
        }
        return false;
    }
}
