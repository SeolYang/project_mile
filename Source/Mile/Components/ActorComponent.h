#pragma once
#include "Mile.h"

namespace Mile
{
    class Actor;

    /**
    * ActorComponent는 Actor에 추가될 수 있는 재사용 가능한 동작을 정의하는 Component의 기본 클래스입니다.
    */
    class MILE_API ActorComponent
    {
    public:
        ActorComponent( Actor* Owner ) :
            bIsTick( false ),
            Owner( Owner )
        {
        }

        Actor* GetOwner( ) const
        {
            return Owner;
        }

    public:
        bool bIsTick;

    private:
        Actor* Owner;
        bool bIsRegistered;

    };
}