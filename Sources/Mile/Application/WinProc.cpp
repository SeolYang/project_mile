#include "WinProc.h"

LRESULT WinProc( HWND Handle, UINT Msg, WPARAM WParam, LPARAM LParam )
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