#pragma once
#include "Object.h"

namespace Mile
{
    /**
    * ActorComponent는 Actor에 추가될 수 있는 재사용 가능한 동작을 정의하는 Component의 기본 클래스입니다.
    */
    class Actor;
    class MILE_API ActorComponent : public Object
    {
        friend Actor;
    public:
        ActorComponent( const MString& NewName, Actor* NewOwner ) :
            Owner( NewOwner ),
            bIsRegistered( true ),
            bIsTick( false ),
            Object( NewName )
        {
        }

        /* @todo: 이미 Owner가 유효한 상태에서 호출시 인수인계 작업 구현 */
        void SetOwner( Actor* NewOwner );
        Actor* GetOwner( ) const
        {
            return Owner;
        }

        virtual void TickComponent( float DeltaTime ) {}

        FORCEINLINE bool IsRegistered( ) const
        {
            return bIsRegistered;
        }

    protected:
        void OnAttachPost( )
        {
            bIsRegistered = true;
            OnAttach( );
        }

        void OnDetachPost( )
        {
            bIsRegistered = false;
            OnDetach( );
        }

        virtual void OnAttach( ) {}
        virtual void OnDetach( ) {}

        void DetachFromOwner( );

    private:
        bool bIsRegistered;
        Actor* Owner;

    public:
        bool    bIsTick;
        uint64  TickPriority;

    };
}