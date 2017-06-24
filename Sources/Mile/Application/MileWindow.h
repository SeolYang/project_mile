#pragma once
#include "MileObject.h"
#include "MileString.h"

namespace Mile
{
    enum class MILE_API EWindowMode
    {
        Windowed,
        FullWindowed,
        FullScreen
    };

    /**
    * 창을 나타내는 클래스
    */
    class MILE_API Window
    {
    public:

        /**
        * 윈도우의 생성자
        * @param NewTitle 설정할 윈도우의 타이틀
        * @param NewWidth 설정할 윈도우의 너비
        * @param NewHeight 설정할 윈도우의 높이
        * @param NewWindowMode 설정할 윈도우 모드
        */
        Window( const MString& NewTitle,
            uint32 NewWidth,
            uint32 NewHeight,
            EWindowMode NewWindowMode ) :
            Width( NewWidth ), Height( NewHeight ),
            WindowMode( NewWindowMode )
        {
        }

        /**
        * 윈도우의 소멸자
        */
        virtual ~Window( ) {}

        /**
        * 윈도우의 타이틀을 가져옵니다.
        */
        MString GetTitle( ) const { return GetName( ); }

        /**
        * 윈도우의 현재 너비를 가져옵니다.
        * @return 윈도우의 현재 너비.
        */
        FORCEINLINE uint32 GetWidth( ) const { return Width; }
        /**
        * 윈도우의 현재 높이를 가져옵니다.
        * @return 윈도우의 현재 높이.
        */
        FORCEINLINE uint32 GetHeight( ) const { return Height; }

        /**
        * 윈도우의 현재 윈도우 모드를 가져옵니다.
        * @return 윈도우의 현재 윈도우의 모드
        */
        FORCEINLINE EWindowMode GetWindowMode( ) const { return WindowMode; }

        /**
        * 윈도우를 리사이즈 하는 메소드입니다.
        * @param NewWidth 새로 설정할 너비
        * @param NewHeight 새로 설정할 높이
        */
        void Resize( uint32 NewWidth, uint32 NewHeight );

        /**
        * 윈도우의 윈도우 모드를 설정합니다.
        * @param NewWindowMode 새로 설정할 윈도우 모드
        */
        void SetWindowMode( EWindowMode NewWindowMode );

        virtual bool Initialize( ) = 0;
        virtual void DeInitialize( ) = 0;

        /**
        * 윈도우의 크기가 재설정되었을 때 호출되는 콜백 함수입니다.
        * @param NewWidth 새로 설정된 너비
        * @param NewHeight 새로 설정된 높이
        */
        virtual void OnResize( uint32 NewWidth, uint32 NewHeight ) = 0;

        /**
        * 윈도우의 윈도우 모드가 재설정되었을 때 호출되는 콜백 함수입니다.
        * @param NewWindowMode 새로 설정된 모드
        */
        virtual void OnWindowModeChange( EWindowMode NewWindowMode ) = 0;

    private:
        uint32 Width;
        uint32 Height;

        EWindowMode WindowMode;

    };
}