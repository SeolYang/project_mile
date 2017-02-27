#pragma once
#include "Mile.h"
#include "MileString.h"
#include "SceneComponent.h"

namespace Mile
{
    class Actor
    {
    public:
        Actor( const MString& Name ) :
            Name( Name ), RootComponent( nullptr )
        {
        }

        virtual void Tick( float DeltaTime ) { }
        virtual void TickActor( float DeltaTime ) final;

    public:
        bool bIsTick;

    protected:
        MString Name;
        SceneComponent* RootComponent;

    private:
        std::vector<ActorComponent*> OwnedComponent;
        std::vector<Actor*> Children;

    };
}