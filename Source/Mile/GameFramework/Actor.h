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
            Name( Name ), RootComponent( std::make_unique<SceneComponent>( ) )
        {
        }

    protected:
        MString Name;
        std::unique_ptr<SceneComponent> RootComponent;

    };
}