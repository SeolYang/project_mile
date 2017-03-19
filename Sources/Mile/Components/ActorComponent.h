#pragma once
#include "MileObject.h"

namespace Mile
{
    /**
    * ActorComponent는 Actor에 추가될 수 있는 재사용 가능한 동작을 정의하는 Component의 기본 클래스입니다.
    */
    class Actor;
    class MILE_API ActorComponent : public Mile::Object
    {
        friend Actor;
    public:
        /**
        * ActorComponent의 기본 생성자
        * @param NewName 설정될 오브젝트 이름
        */
        ActorComponent( const MString& NewName ) :
            bIsTick( false ),
            OwnerPrivate( nullptr ),
            TickPriority( 0 ),
            Object( NewName )
        {
        }

        /**
        * ActorComponent의 이동 생성자
        * @param MovedObject 이동될 오브젝트
        */
        ActorComponent( ActorComponent&& MovedObject ) :
            OwnerPrivate( MovedObject.OwnerPrivate ),
            bIsTick( MovedObject.bIsTick ),
            TickPriority( MovedObject.TickPriority ),
            Object( std::move( MovedObject ) )
        {
            this->SetOwner( MovedObject.GetOwner( ) );
            MovedObject.SetOwner( nullptr );
            MovedObject.bIsTick = false;
            MovedObject.TickPriority = UINT64_MAX;
        }

        /**
        * ActorComponent의 이동 대입 연산자
        * @param MovedObject 이동될 오브젝트
        * @return 이동된 오브젝트
        */
        ActorComponent& operator=( ActorComponent&& MovedObject ) = delete;

        /**
        * ActorComponent가 붙어있는 주체(주인)을 설정한다.
        * @param Owner 새로 주인으로 설정할 액터
        * @param bIsDetachBeforeSetOwner 만약 이 값이 true이면 새로운 주인을 설정하기 전에 기존의 주인에게서 분리한다.
        */
        void SetOwner( Actor* Owner, bool bIsDetachBeforeSetNewOwner = true );

        /**
        * 호출한 ActorComponent 객체와 그 객체의 자식 Component들의 주인들을 일괄적으로 설정한다.
        * @param Owner 새로 주인으로 설정할 액터
        * @param bIsDetachBeforeSetOwner 만약 이 값이 true이면 새로운 주인을 설정하기 전에 기존의 주인에게서 분리한다.
        */
        virtual void SetOwnerRecursively( Actor* NewOwner, bool bIsDetachBeforeSetNewOwner = true );

        /**
        * 현재 붙어있는 주인을 반환한다.
        * @return 현재 주인으로 설정되어있는 Actor
        */
        FORCEINLINE Actor* GetOwner( ) const
        {
            return OwnerPrivate;
        }

        /**
        * 매 프레임마다 호출되는 메소드
        * @param DeltaTime 프레임과 프레임사이에 흐른 시간
        */
        virtual void Tick( float DeltaTime ) { UNUSED_PARAM( DeltaTime ); }

    private:
        Actor* OwnerPrivate;
        bool    bIsTick;
        uint64  TickPriority;

    };
}