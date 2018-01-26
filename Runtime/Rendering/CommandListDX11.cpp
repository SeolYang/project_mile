#include "CommandListDX11.h"
namespace Mile
{
   CommandListDX11::CommandListDX11( RendererDX11* renderer ) :
      m_renderer( renderer ),
      m_bIsInitialized( false ),
      m_list( nullptr )
   {
   }

   CommandListDX11::~CommandListDX11( )
   {
      SafeRelease( m_list );
   }

   bool CommandListDX11::Init( ID3D11DeviceContext* deviceContext )
   {
      if ( m_renderer == nullptr || m_bIsInitialized )
      {
         return false;
      }

      m_bIsInitialized = true;
      return true;
   }

   void CommandListDX11::DeInit( )
   {
      SafeRelease( m_list );
      m_bIsInitialized = false;
   }

   bool CommandListDX11::Execute( ID3D11DeviceContext* deviceContext, bool restoreContextState )
   {
      if ( !m_bIsInitialized || deviceContext == nullptr )
      {
         return false;
      }

      deviceContext->ExecuteCommandList( m_list, restoreContextState );

      return true;
   }
}