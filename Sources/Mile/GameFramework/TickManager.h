#pragma once
#include "MileObject.h"
#include <vector>
#include <functional>

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
    class MILE_API TickManager final : public Mile::Object
    {
    public:
        ~TickManager( );

        static TickManager& GetInstance( );
        bool DestroyInstance( );

        void AddEvent( TickFuncType Func, uint64 ObjectID, uint64 Priority = 0 );
        void RemoveEvent( uint64 ObjectID );
        void ModifyPriority( uint64 ObjectID, uint64 Priority );

        void Tick( float DeltaTime );

    private:
        TickManager( );

        void OrderingPriority( uint64 AddedPriority, bool ForceOrdering = false );
        bool IsAlreadyAdded( uint64 ObjectID ) const;

    private:
        static TickManager* Instance;
        ContainerType Container;
        uint64 RecentAddedPriority;

    };
}