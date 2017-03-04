#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"

namespace Mile
{
    void Actor::AttachComponent( ActorComponent* Component )
    {
        bool bIsValidComponent =
            Component != nullptr && ( Component->GetOwner( ) != this );

        if ( bIsValidComponent )
        {
            std::wcout << Component->GetName( ).operator std::wstring( ) << TEXT( " is attached to " ) << GetName( ).operator std::wstring() << std::endl;
            Component->SetOwnerRecursively( this );
            Components.push_back( Component );
        }
    }

    void Actor::DetachComponent( ActorComponent* Component )
    {
        bool bIsValidComponent =
            ( Component != nullptr ) && ( Component->GetOwner( ) == this );
        if ( bIsValidComponent )
        {
            for ( auto FoundComponent = Components.begin( );
                FoundComponent != Components.end( );
                ++FoundComponent )
            {
                if ( ( *FoundComponent ) == Component )
                {
                    Components.erase( FoundComponent );
                    Component->SetOwner( nullptr, false );
                    std::wcout << Component->GetName( ).operator std::wstring( ) << TEXT( " has dettached." ) << std::endl;
                    return;
                }
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
