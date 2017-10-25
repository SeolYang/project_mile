#include "Engine.h"
#include "Context.h"
#include "Resource\ResourceManager.h"
#include "World.h"

namespace Mile
{
    Engine::Engine( Context* context ) :
        SubSystem( context )
    {
        m_context->RegisterSubSystem( this );

        m_resourceManager = new ResourceManager( m_context );
        m_context->RegisterSubSystem( m_resourceManager );

        m_world = new World( m_context );
        m_context->RegisterSubSystem( m_world );
    }

    bool Engine::Initialize( )
    {
        // Initialize subsystems

        // Initialize Resource manager
        if ( !m_context->GetSubSystem<ResourceManager>( )->Initialize( ) )
        {
            return false;
        }

        // Initialize World
        if ( !m_context->GetSubSystem<World>( )->Initialize( ) )
        {
            //@TODO: Add Log
            return false;
        }

        return true;
    }

    void Engine::Update( )
    {
        // Update subsystems
        m_world->Update( );
    }

    void Engine::ShutDown( )
    {
        m_world = nullptr;
        SafeDelete( m_context );
    }
}