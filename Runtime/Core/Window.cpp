#include "Window.h"
#include "Context.h"
#include "Config.h"

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
   }

   bool Window::Initialize( )
   {
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
                               WS_POPUP | WS_CAPTION | WS_VISIBLE,
                               posX, posY, m_resWidth, m_resHeight,
                               nullptr, nullptr, nullptr, nullptr );

      return true;
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