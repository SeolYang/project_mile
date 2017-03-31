#pragma once
#include "MileManager.hpp"

namespace Mile
{
    using TickFuncType = std::function<void( float )>;
    /* 높은 값일수록 더 높은 우선순위를 가짐. */
    struct MILE_API TickFunction
    {
        TickFuncType Func;
        uint64 ObjectID;
        uint64 Priority;
    };

    using ContainerType = std::vector<TickFunction>;

    /**
    * Tick 이벤트를 관리하는 매니저 클래스
    */
    class MILE_API TickManager final : public Mile::Manager<TickManager>
    {
        friend Manager;

    public:
        TickManager( const TickManager& ) = delete;
        TickManager& operator=( const TickManager& ) = delete;

        void AddEvent( TickFuncType Func, uint64 ObjectID, uint64 Priority = 0 );
        void RemoveEvent( uint64 ObjectID );
        void ModifyPriority( uint64 ObjectID, uint64 Priority );

        void Tick( float DeltaTime );

    private:
        TickManager( );
        ~TickManager( );

        void OrderingPriority( uint64 AddedPriority, bool ForceOrdering = false );
        bool IsAlreadyAdded( uint64 ObjectID ) const;

    private:
        ContainerType Container;
        uint64 RecentAddedPriority;

    };
}