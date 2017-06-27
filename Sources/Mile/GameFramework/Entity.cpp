#include "Entity.h"
#include "World.h"
#include "Component.h"

namespace Mile
{
    void Entity::SetWorld( Mile::World* NewWorld )
    {
        World = NewWorld;
        bIsRegisteredAtWorld = ( World != nullptr );
    }

    Entity::~Entity( )
    {
        // Entity has responsibility to delete Components.
        for ( auto Comp : Components )
        {
            delete Comp.second;
        }
    }

    void Entity::DetachComponent( uint64 TargetObjectID )
    {
        auto FoundItr = Components.FindIf(
            [ TargetObjectID ]( Rumia::Pair<size_t, Component*> Pair )->bool 
        { return ( Pair.first == TargetObjectID ); } );

        DetachComponent( FoundItr );
    }

    void Entity::DetachComponent( Component* Target )
    {
        auto FoundItr = Components.FindIf(
            [ Target ]( Rumia::Pair<size_t, Component*> Pair )->bool
        {
            return ( Pair.second == Target );
        } );

        DetachComponent( FoundItr );
    }

    void Entity::DetachComponent( ComponentContainerType::iterator TargetItr )
    {
        Components.Erase( TargetItr );
        ( *TargetItr ).second->SetOwner( nullptr );
    }

    Component* Entity::GetComponent( uint64 TargetObjectID )
    {
        auto FoundItr = Components.FindIf(
            [ TargetObjectID ]( Rumia::Pair<size_t, Component*> Pair )->bool
        {
            return ( Pair.second->GetObjectID( ) == TargetObjectID );
        } );

        if ( FoundItr != Components.end( ) )
        {
            return ( *FoundItr ).second;
        }

        return nullptr;
    }

    void Entity::AttachEntity( Entity* NewChild )
    {
        if ( NewChild != nullptr )
        {
            Entity* OldParent = &NewChild->GetParent( );
            if ( OldParent != this )
            {
                if ( OldParent != nullptr )
                {
                    OldParent->DetachEntity( NewChild );
                }

                Children.PushBack( NewChild );
                NewChild->Parent = this;
                World->RegisterEntity( NewChild );
            }
        }
    }

    void Entity::DetachEntity( Entity* Target )
    {
        if ( Target != nullptr )
        {
            auto Child = Children.Find( Target );
            if ( Child != Children.end( ) )
            {
                Children.Erase( Child );
                ( *Child )->Parent = nullptr;
                World->UnRegisterEntity( ( *Child ) );
            }
        }
    }

    void Entity::OnBegin( )
    {
        for ( auto Comp : Components )
        {
            Comp.second->OnBegin( );
        }
    }

    void Entity::OnEnd( )
    {
        for ( auto Comp : Components )
        {
            Comp.second->OnEnd( );
        }
    }
}
