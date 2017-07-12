#include "Engine.h"
#include "Context.h"
#include "World.h"

namespace Mile
{
    Engine::Engine( Context* context ) :
        SubSystem( context )
    {
        auto allocator = m_context->GetAllocator( );

        m_context->RegisterSubSystem( this );

        m_world = RUMIA_NEW( allocator, World, m_context );
        m_context->RegisterSubSystem( m_world );
    }

    bool Engine::Initialize( )
    {
        // Initialize subsystems
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
        RUMIA_DELETE( m_context->GetAllocator( ), m_context );
    }
}