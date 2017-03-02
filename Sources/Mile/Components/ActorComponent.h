#pragma once
#include "MileObject.h"

namespace Mile
{
    /**
    * ActorComponent는 Actor에 추가될 수 있는 재사용 가능한 동작을 정의하는 Component의 기본 클래스입니다.
    */
    class Actor;
    class MILE_API ActorComponent : public MileObject
    {
        friend Actor;
    public:
        ActorComponent( const MString& NewName ) :
            bIsTick( false ),
            OwnerPrivate( nullptr ),
            MileObject( NewName )
        {
        }

        void SetOwner( Actor* Owner );
        FORCEINLINE Actor* GetOwner( ) const
        {
            return OwnerPrivate;
        }

        virtual void TickComponent( float DeltaTime ) {}

    private:
        Actor* OwnerPrivate;

    public:
        bool    bIsTick;
        uint64  TickPriority;

    };
}