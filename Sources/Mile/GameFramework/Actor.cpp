#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"
#include "TickManager.h"

namespace Mile
{
    void Actor::AttachComponent( ActorComponent* Component )
    {
        bool bIsValidComponent =
            Component != nullptr && ( Component->GetOwner( ) != this );

        if ( bIsValidComponent )
        {
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

    void Actor::SetIsTick( bool bNewIsTick )
    {
        TickManager& Manager = TickManager::GetInstance( );
        switch ( bNewIsTick )
        {
        case true:
            if ( !bIsTickFuncRegistered )
            {
                Manager.AddEvent( std::bind( &Actor::Tick, this, std::placeholders::_1 ), GetObjectID( ), TickPriority );
                bIsTickFuncRegistered = true;
            }

            break;

        case false:
            if ( bIsTickFuncRegistered )
            {
                Manager.RemoveEvent( GetObjectID( ) );
                bIsTickFuncRegistered = false;
            }

            break;
        }
    }

    void Actor::SetTickPriority( uint64 NewTickPriority )
    {
        if ( bIsTickFuncRegistered )
        {
            TickManager::GetInstance( ).ModifyPriority( GetObjectID( ), NewTickPriority );
        }

        TickPriority = NewTickPriority;
    }

    Vector Actor::GetPosition( ETransformRelation Relation )
    {
        if ( RootComponent != nullptr )
        {
            return RootComponent->GetPosition( Relation );
        }

        return Vector( );
    }

    Vector Actor::GetRotation( ETransformRelation Relation )
    {
        if ( RootComponent != nullptr )
        {
            return RootComponent->GetRotation( Relation );
        }

        return Vector( );
    }

    Vector Actor::GetScale( ETransformRelation Relation )
    {
        if ( RootComponent != nullptr )
        {
            RootComponent->GetScale( Relation );
        }

        return Vector( );
    }

}
