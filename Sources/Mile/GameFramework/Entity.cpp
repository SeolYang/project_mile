#include "Entity.h"
#include "Component.h"

namespace Mile
{
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
}
