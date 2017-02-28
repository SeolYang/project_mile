#pragma once
#include "Object.h"

namespace Mile
{
    class ActorComponent;
    class SceneComponent;
    class Actor : public Object
    {
        using ComponentList = std::vector<ActorComponent*>;
    public:
        Actor( const MString& NewName ) :
            bIsTick( false ),
            bIsChangedAtComponents( true ),
            Object( NewName )
        {
        }

        void PostTick( float DeltaTime );
        void TickActor( float DeltaTime );
        virtual void Tick( float DeltaTime ) { UNUSED_PARAM( DeltaTime ); }

        void AttachComponent( ActorComponent* Target );
        bool DetachComponent( const ActorComponent* Target );

        void AttachRootComponent( SceneComponent* Target );
        bool DetachRootComponent( );
        SceneComponent* GetRootComponent( ) const;

    private:
        ComponentList       Components;
        SceneComponent*     RootComponent;

    public:
        bool                bIsTick;
        bool                bIsChangedAtComponents;

    };
}