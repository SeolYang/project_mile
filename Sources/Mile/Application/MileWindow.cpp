#include "MileWindow.h"

namespace Mile
{
    void Window::Resize( uint32 NewWidth, uint32 NewHeight )
    {
        OnResize( NewWidth, NewHeight );
        Width = NewWidth;
        Height = NewHeight;
    }

    void Window::SetWindowMode( EWindowMode NewWindowMode )
    {
        WindowMode = NewWindowMode;
        OnWindowModeChange( NewWindowMode );
    }
}