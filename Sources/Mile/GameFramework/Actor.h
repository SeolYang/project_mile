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
            bIsChangedAtComponents( true ),
            ParentPrivate( nullptr ),
            MileObject( NewName )
        {
        }

        virtual void Tick( float DeltaTime ) { UNUSED_PARAM( DeltaTime ); }

        void AddComponent( ActorComponent* Component );
        void RemoveComponent( ActorComponent* Component );

        bool AttachToComponent( SceneComponent* Parent );
        void DetachFromComponent( );

        void AttachToActor( Actor* Parent );
        void DetachFromParent( );

        bool SetRootComponent( SceneComponent* NewRootComponent );
        FORCEINLINE SceneComponent* GetRootComponent( ) const { return RootComponent; }
        FORCEINLINE SceneComponent* GetDefaultAttachComponent( ) const { return RootComponent; }

        FORCEINLINE Actor* GetParent( ) { return ParentPrivate; }

    private:
        ComponentList       Components;
        SceneComponent*     RootComponent;
        Actor*              ParentPrivate;

    public:
        bool                bIsTick;
        bool                bIsChangedAtComponents;

    };
}