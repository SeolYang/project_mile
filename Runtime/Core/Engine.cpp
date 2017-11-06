#include "Engine.h"
#include "Context.h"
#include "Config.h"
#include "Window.h"
#include "Resource\ResourceManager.h"
#include "World.h"

namespace Mile
{
   Engine::Engine( Context* context ) :
      SubSystem( context ), m_bIsRunning( false )
   {
      m_context->RegisterSubSystem( this );

      m_configSys = new ConfigSystem( m_context );
      m_context->RegisterSubSystem( m_configSys );

      m_window = new Window( m_context );
      m_context->RegisterSubSystem( m_window );

      m_resourceManager = new ResourceManager( m_context );
      m_context->RegisterSubSystem( m_resourceManager );

      m_world = new World( m_context );
      m_context->RegisterSubSystem( m_world );
   }

   bool Engine::Init( )
   {
      // -* Initialize subsystems *-

      // Initialize ConfigSystem
      if ( !m_context->GetSubSystem<ConfigSystem>( )->Init( ) )
      {
         return false;
      }

      // Initialize Window Subsystem
      if ( !m_context->GetSubSystem<Window>( )->Init( ) )
      {
         return false;
      }

      // Initialize Resource manager
      if ( !m_context->GetSubSystem<ResourceManager>( )->Init( ) )
      {
         return false;
      }

      // Initialize World
      if ( !m_context->GetSubSystem<World>( )->Init( ) )
      {
         //@TODO: Add Log
         return false;
      }

      return true;
   }

   int Engine::Execute( )
   {
      m_bIsRunning = true;

      while ( m_bIsRunning )
      {
         this->Update( );
      }

      return 0;
   }

   void Engine::Update( )
   {
      // Update subsystems
      m_window->Update( );
      m_world->Update( );
   }

   void Engine::ShutDown( )
   {
      m_bIsRunning = false;
      m_world = nullptr;
      SafeDelete( m_context );
   }
}