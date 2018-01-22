#include "Window.h"
#include "Context.h"
#include "Engine.h"
#include "Config.h"
#include "Logger.h"

namespace Mile
{
   Window::Window( Context* context ) : SubSystem( context ),
      m_handle( nullptr ),
      m_resWidth( 0 ), m_resHeight( 0 ),
      m_refreshRate( 60.0 ), m_bIsFullScreen( false )
   {
   }

   Window::~Window( )
   {
      DeInit( );
   }

   bool Window::Init( )
   {
      if ( m_context == nullptr || m_bIsInitialized )
      {
         MELog( m_context, TEXT( "Window" ), ELogType::WARNING, TEXT( "Window already initialized." ), true );
         return false;
      }

      auto configSys = m_context->GetSubSystem<ConfigSystem>( );

      auto config = configSys->GetConfig( TEXT( "Engine" ) );
      std::wstring windowTitle = String2WString( config.second[ "Title" ] );
      
      WNDCLASS wndClass = { 0 };
      wndClass.style = CS_OWNDC;
      wndClass.lpfnWndProc = &Mile::WinProc;
      wndClass.hCursor = LoadCursor( nullptr, IDC_ARROW );
      wndClass.lpszClassName = windowTitle.c_str( );
      RegisterClass( &wndClass );

      unsigned int posX = config.second[ "PosX" ];
      unsigned int posY = config.second[ "PosY" ];
      m_resWidth = config.second[ "ResolutionWidth" ];
      m_resHeight = config.second[ "ResolutionHeight" ];

      m_handle = CreateWindow( windowTitle.c_str( ), windowTitle.c_str( ),
                               WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
                               posX, posY, m_resWidth, m_resHeight,
                               nullptr, nullptr, nullptr, nullptr );

      MELog( m_context, TEXT( "Window" ), ELogType::MESSAGE, TEXT( "Window Initialized!" ), true );
      m_bIsInitialized = true;
      return true;
   }

   void Window::DeInit( )
   {
      if ( m_bIsInitialized )
      {
         MELog( m_context, TEXT( "Window" ), ELogType::MESSAGE, TEXT( "Window deinitialized." ), true );

         SubSystem::DeInit( );
      }
   }

   void Window::Update( )
   {
      MSG msg = { 0 };
      if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
      {
         auto message = msg.message;
         if ( message == WM_QUIT || message == WM_CLOSE || message == WM_DESTROY )
         {
            // Dummy flag for shutting down after 1 frame
            m_context->GetSubSystem<Engine>( )->ShutDownFlagEnable( );
         }

         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
   }

   LRESULT WinProc( HWND Handle, unsigned int Msg, WPARAM wParam, LPARAM lParam )
   {
      switch ( Msg )
      {
      case WM_DESTROY:
      case WM_CLOSE:
         PostQuitMessage( 0 );
         break;
      }

      return DefWindowProc( Handle, Msg, wParam, lParam );
   }
}