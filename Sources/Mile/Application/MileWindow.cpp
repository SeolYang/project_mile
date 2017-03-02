#include "MileWindow.h"

namespace Mile
{
    void MileWindow::Resize( uint64 NewWidth, uint64 NewHeight )
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