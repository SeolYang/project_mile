#include "TickManager.h"
#include <algorithm>

namespace Mile
{
    TickManager* TickManager::Instance = nullptr;

    TickManager::TickManager( ) :
        RecentAddedPriority( 0 )
    {
    }

    TickManager::~TickManager( )
    {
        Container.clear( );
    }

    TickManager& TickManager::GetInstance( )
    {
        if ( Instance == nullptr )
        {
            Instance = new TickManager( );
        }

        return ( *Instance );
    }

    void TickManager::AddEvent( TickFuncType Func, uint64 ObjectID, uint64 Priority )
    {
        if ( !IsAlreadyAdded( ObjectID ) )
        {
            TickFunction FuncObject;
            FuncObject.Func = Func;
            FuncObject.ObjectID = ObjectID;
            FuncObject.Priority = Priority;
            Container.push_back( FuncObject );

            OrderingPriority( Priority );
        }
    }

    void TickManager::RemoveEvent( uint64 ObjectID )
    {
        for( auto TickObject = Container.begin( );
            TickObject != Container.end( );
            ++TickObject )
        {
            if ( ( *TickObject ).ObjectID == ObjectID )
            {
                Container.erase( TickObject );
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

        OrderingPriority(Priority, true);
    }

    void TickManager::Tick(float DeltaTime)
    {
        for (auto TickObject : Container)
        {
            TickObject.Func(DeltaTime);
        }
    }

    bool TickManager::DestroyInstance( )
    {
        if ( Instance != nullptr )
        {
            delete Instance;
            Instance = nullptr;
            return true;
        }

        return false;
    }

    void TickManager::OrderingPriority( uint64 AddedPriority, bool ForceOrdering)
    {
        if ( (RecentAddedPriority != AddedPriority) || ForceOrdering )
        {
            RecentAddedPriority = AddedPriority;
            
            std::sort( Container.begin( ), Container.end( ),
                [ ]( TickFunction Left, TickFunction Right )->bool
            {
                return ( Left.Priority > Right.Priority );
            } );
        }
    }

    bool TickManager::IsAlreadyAdded( uint64 ObjectID ) const
    {
        for( const auto Object : Container )
        {
            if ( Object.ObjectID == ObjectID )
            {
                return true;
            }
        }

        return false;
    }
}