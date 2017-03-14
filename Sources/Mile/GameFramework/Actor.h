#pragma once
#include "MileObject.h"

namespace Mile
{
    class ActorComponent;
    class SceneComponent;
    class MILE_API Actor : public MileObject
    {
        using ComponentList = std::vector<ActorComponent*>;
    public:
        Actor( const MString& NewName ) :
            bIsTick( false ),
            bIsTickFuncRegistered( false ),
            bIsChangedAtComponents( true ),
            ParentPrivate( nullptr ),
            TickPriority( 0 ),
            MileObject( NewName )
        {
        }

        virtual void Tick( float DeltaTime ) { UNUSED_PARAM( DeltaTime ); }

        void AttachComponent( ActorComponent* Component );
        void DetachComponent( ActorComponent* Component );

        bool AttachToComponent( SceneComponent* Parent );
        void DetachFromComponent( );

        void AttachToActor( Actor* Parent );
        void DetachFromParent( );

        bool SetRootComponent( SceneComponent* NewRootComponent );
        FORCEINLINE SceneComponent* GetRootComponent( ) const { return RootComponent; }
        FORCEINLINE SceneComponent* GetDefaultAttachComponent( ) const { return RootComponent; }

        FORCEINLINE Actor* GetParent( ) { return ParentPrivate; }

        void SetIsTick( bool bNewIsTick );
        FORCEINLINE bool IsTick( ) const { return bIsTick; }

        void SetTickPriority( uint64 NewTickPriority );
        FORCEINLINE uint64 GetTickPriority( ) const { return TickPriority; }

    private:
        ComponentList       Components;
        SceneComponent*     RootComponent;
        Actor*              ParentPrivate;
        bool                bIsTick;
        bool                bIsTickFuncRegistered;
        uint64              TickPriority;

    public:
        bool                bIsChangedAtComponents;

    };
}