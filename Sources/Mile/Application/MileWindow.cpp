#include "MileWindow.h"

namespace Mile
{
    void MileWindow::Resize( uint32 NewWidth, uint32 NewHeight )
    {
        OnResize( NewWidth, NewHeight );
        Width = NewWidth;
        Height = NewHeight;
    }

    void MileWindow::SetWindowMode( EWindowMode NewWindowMode )
    {
        WindowMode = NewWindowMode;
        OnWindowModeChange( NewWindowMode );
    }
}