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
            uint64 NewWidth,
            uint64 NewHeight,
            EWindowMode NewWindowMode ) :
            Title( NewTitle ),
            Width( NewWidth ), Height( NewHeight ),
            WindowMode( NewWindowMode )
        {
        }

        virtual ~MileWindow( ) {}


        MString GetTitle( ) const { return Title; }

        FORCEINLINE uint64 GetWidth( ) const { return Width; }
        FORCEINLINE uint64 GetHeight( ) const { return Height; }

        FORCEINLINE EWindowMode GetWindowMode( ) const { return WindowMode; }

        void Resize( uint64 NewWidth, uint64 NewHeight );
        void SetWindowMode( EWindowMode NewWindowMode );

        virtual bool Initialize( ) = 0;
        virtual void DeInitialize( ) = 0;
        virtual void OnResize( uint64 NewWidth, uint64 NewHeight ) = 0;
        virtual void OnWindowModeChange( EWindowMode NewWindowMode ) = 0;

    private:
        MString Title;

        uint64 Width;
        uint64 Height;

        EWindowMode WindowMode;

    };
}