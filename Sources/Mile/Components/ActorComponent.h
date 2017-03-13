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
            TickPriority( 0 ),
            MileObject( NewName )
        {
        }

        ActorComponent( ActorComponent&& MovedObject ) :
            OwnerPrivate( MovedObject.OwnerPrivate ),
            bIsTick( MovedObject.bIsTick ),
            TickPriority( MovedObject.TickPriority ),
            MileObject( std::move( MovedObject ) )
        {
            this->SetOwner( MovedObject.GetOwner( ) );
            MovedObject.SetOwner( nullptr );
            MovedObject.bIsTick = false;
            MovedObject.TickPriority = UINT64_MAX;
        }

        ActorComponent& operator=( ActorComponent&& MovedObject ) = delete;

        void SetOwner( Actor* Owner, bool bIsDetachBeforeSetNewOwner = true );
        virtual void SetOwnerRecursively( Actor* NewOwner, bool bIsDetachBeforeSetNewOwner = true );
        FORCEINLINE Actor* GetOwner( ) const
        {
            return OwnerPrivate;
        }

        virtual void Tick( float DeltaTime ) { UNUSED_PARAM( DeltaTime ); }

    private:
        Actor* OwnerPrivate;
        bool    bIsTick;
        uint64  TickPriority;

    };
}