#include "Engine.h"
#include "Context.h"

namespace Mile
{
    Engine::Engine( Context* context ) :
        SubSystem( context )
    {
        m_context->RegisterSubSystem( this );
    }

    bool Engine::Initialize( )
    {
        return false;
    }

    void Engine::Update( )
    {

    }

    void Engine::PostUpdate( )
    {

    }

    void Engine::ShutDown( )
    {

    }
}