#include "TickManager.h"

namespace Mile
{
    TickManager::TickManager( ) :
        RecentAddedPriority( 0 ),
        Mile::Manager<TickManager>( )
    {
    }

    TickManager::~TickManager( )
    {
        Container.Clear( );
    }

    void TickManager::AddEvent( TickFuncType Func, uint64 ObjectID, uint64 Priority )
    {
        if ( !IsAlreadyAdded( ObjectID ) )
        {
            TickFunction FuncObject;
            FuncObject.Func = Func;
            FuncObject.ObjectID = ObjectID;
            FuncObject.Priority = Priority;
            Container.PushBack( FuncObject );

            OrderingPriority( Priority );
        }
    }

    void TickManager::RemoveEvent( uint64 ObjectID )
    {
        for ( auto TickObject = Container.begin( );
              TickObject != Container.end( );
              ++TickObject )
        {
            if ( ( *TickObject ).ObjectID == ObjectID )
            {
                Container.Erase( TickObject );
                return;
            }
        }
    }

    void TickManager::ModifyPriority( uint64 ObjectID, uint64 Priority )
    {
        for ( auto& Object : Container )
        {
            if ( Object.ObjectID == ObjectID )
            {
                Object.Priority = Priority;
            }
        }

        OrderingPriority( Priority, true );
    }

    void TickManager::Tick( float DeltaTime )
    {
        for ( auto TickObject : Container )
        {
            TickObject.Func( DeltaTime );
        }
    }

    void TickManager::OrderingPriority( uint64 AddedPriority, bool ForceOrdering )
    {
        if ( ( RecentAddedPriority != AddedPriority ) || ForceOrdering )
        {
            RecentAddedPriority = AddedPriority;

            //@TODO: Implement own sorting algorithm
            //std::sort( Container.begin( ), Container.end( ),
            //           [ ]( TickFunction Left, TickFunction Right )->bool
            //{
            //    return ( Left.Priority > Right.Priority );
            //} );
        }
    }

    bool TickManager::IsAlreadyAdded( uint64 ObjectID ) const
    {
        for ( auto Itr = Container.cbegin( ); Itr != Container.cend( ); ++Itr )
        {
            if ( (*Itr).ObjectID == ObjectID )
            {
                return true;
            }
        }

        return false;
    }
}