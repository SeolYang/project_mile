#pragma once
#include "MileWindow.h"

namespace Mile
{
    /**
    * Win32 플랫폼에서 동작하는 윈도우를 나타내는 클래스
    */
    class MILE_API Win32Window : public Window
    {
    public:
        Win32Window( const MString& NewTitle,
                     uint32 NewWidth = 800,
                     uint32 NewHeight = 600,
                     EWindowMode NewWindowMode = EWindowMode::Windowed ) :
            Handle( nullptr ),
            Window(
                NewTitle,
                NewWidth, NewHeight,
                NewWindowMode )
        {
        }

        /**
        * Win32 핸들을 반환합니다.
        * @return 반환될 Win32 Handle
        */
        FORCEINLINE HWND GetHandle( ) const { return Handle; }

        virtual bool Initialize( ) override;
        virtual void DeInitialize( ) override;
        virtual void OnResize( uint32 NewWidth, uint32 NewHeight ) override;
        virtual void OnWindowModeChange( EWindowMode NewWindowMode ) override;

    private:
        HWND Handle;

    };
}