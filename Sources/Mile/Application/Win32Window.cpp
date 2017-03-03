#include "Win32Window.h"

/* @testcode */
LRESULT CALLBACK WinProc( HWND Handle, UINT Msg, WPARAM WParam, LPARAM LParam )
{
    switch ( Msg )
    {
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage( 0 );
        return 0;

    case WM_SIZE:

        break;
    }

    return DefWindowProc( Handle, Msg, WParam, LParam );
}
/*************/

namespace Mile
{
    bool Win32Window::Initialize( )
    {
        std::wstring Title = static_cast<std::wstring>( GetTitle( ) );

        /* 여기서 Win32 초기화 */
        WNDCLASS WinClass = { NULL };
        WinClass.style = CS_OWNDC;
        WinClass.lpfnWndProc = &WinProc;

        /* 나중에 DirectX 에서 커스텀 커서 지원하기 */
        WinClass.hCursor = LoadCursor( nullptr, IDC_ARROW );
        WinClass.lpszClassName = Title.c_str( );
        RegisterClass( &WinClass );

        /* 화면모드에 따른 적절한 처리하기 */
        uint32 Width = GetWidth( );
        uint32 Height = GetHeight( );
        Handle = CreateWindow(
            Title.c_str( ), Title.c_str( ),
            WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
            0, 0, Width, Height, /* @todo: 시작위치 정할수 있도록 하기 */
            nullptr, nullptr, nullptr, nullptr );

        if ( Handle == nullptr )
        {
            ASSERT_MSG( ( true ), TEXT( "Win32Window failed to initialize." ) );
            return false;
        }

        return true;
    }

    void Win32Window::DeInitialize( )
    {
        /* @todo: 할당 해제 작업 */
    }

    void Win32Window::OnResize( uint32 NewWidth, uint32 NewHeight )
    {
        /* @todo: 윈도우 메세지 콜백 후 필요한 내용들 추가적으로 처리. */
    }

    void Win32Window::OnWindowModeChange( EWindowMode NewWindowMode )
    {
        /* @todo: 윈도우 모드가 봐뀔때 추가적인 작업 */
    }
}