#pragma once
#include "MileString.h"
#include "Object.h"

namespace Mile
{
    class ActorComponent;
    class Actor : public Object
    {
    public:
        Actor( const MString& Name ) :
            Name( Name ), RootComponent( nullptr )
        {
        }

        virtual void TickActor( float DeltaTime ) final;

    protected:
        MString Name;
        ActorComponent* RootComponent;

    };
}