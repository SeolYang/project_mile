#pragma once
#include "Mile.h"

namespace Mile
{
    class Object;
    /**
    * Base class of entire component class.
    */
    class MILE_API Component
    {
    public:
        Component( Object* Owner ) :
            bIsTick( false ),
            Owner( Owner )
        {
        }

        /* @todo: 이미 Owner가 유효한 상태에서 호출시 인수인계 작업 구현 */
        void SetOwner( Object* NewOwner );
        Object* GetOwner( ) const
        {
            return Owner;
        }

        virtual void Tick( float DeltaTime ) {}

    public:
        bool bIsTick;

    protected:
        Object* Owner;
        bool bIsRegistered;

    };
}