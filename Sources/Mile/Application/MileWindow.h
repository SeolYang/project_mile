#pragma once
#include "Mile.h"
#include "MileString.h"

namespace Mile
{
    enum class EWindowMode
    {
        Windowed,
        FullWindowed,
        FullScreen
    };

    /**
    * 창을 나타내는 클래스
    */
    class MILE_API MileWindow
    {
    public:
        MileWindow( const MString& NewTitle,
            uint32 NewWidth,
            uint32 NewHeight,
            EWindowMode NewWindowMode ) :
            Title( NewTitle ),
            Width( NewWidth ), Height( NewHeight ),
            WindowMode( NewWindowMode )
        {
        }

        virtual ~MileWindow( ) {}


        MString GetTitle( ) const { return Title; }

        FORCEINLINE uint32 GetWidth( ) const { return Width; }
        FORCEINLINE uint32 GetHeight( ) const { return Height; }

        FORCEINLINE EWindowMode GetWindowMode( ) const { return WindowMode; }

        void Resize( uint32 NewWidth, uint32 NewHeight );
        void SetWindowMode( EWindowMode NewWindowMode );

        virtual bool Initialize( ) = 0;
        virtual void DeInitialize( ) = 0;
        virtual void OnResize( uint32 NewWidth, uint32 NewHeight ) = 0;
        virtual void OnWindowModeChange( EWindowMode NewWindowMode ) = 0;

    private:
        MString Title;

        uint32 Width;
        uint32 Height;

        EWindowMode WindowMode;

    };
}