#pragma once
#include "MileObject.h"

namespace Mile
{
    /**
    * 타이머 클래스입니다.
    */
    class MILE_API Timer final : public Mile::Object
    {
    public:
        ~Timer( );

        /**
        * 타이머를 Tick 시킵니다.
        */
        void Tick( );

        /**
        * 타이머를 초기화 시킵니다.
        */
        void Reset( );

        /**
        * 타이머가 누적 시간을 반환합니다.
        * @return 타이머의 누적 시간
        */
        float GetRuntime( ) const;

        /**
        * 타이머의 델타 시간을 반환합니다. (Tick 메소드 호출 사이의 시간차)
        * @param 델타 시간
        */
        float GetDeltaTime( ) const;

        /**
        * 타이머가 카운트한 프레임수를 반환합니다.
        * @return 프레임 수
        */
        uint64 GetFrameCount( ) const;

        /**
        * 타이머가 마지막으로 측정한 초당 프레임수를 반환합니다.
        * @return 초당 프레임 수
        */
        uint64 GetFPS( )  const;

        /**
        * 타이머가 측정한 초당 프레임수치중 가장 높은 수치를 반환합니다.
        * @return 측정된 수치중 가장큰 초당 프레임 수
        */
        uint64 GetMaxFPS( ) const;

        /**
        * 설정된 Scale에 종속적인 Delta time 을 반환합니다.
        * @return Scaled Delta time
        */
        float GetScaledDeltaTime( ) const;

        /**
        * 설정된 Time scale을 반환합니다.
        * @return Time scale value
        */
        float GetTimeScale( ) const;

        /**
        * Time Scale을 설정합니다.
        * @param Scale 설정할 Scale 값
        */
        void SetTimeScale( float Scale );

    private:
        /**
        * 타이머 클래스의 기본 생성자
        * @param NewName 타이머의 오브젝트 이름
        */
        Timer( const class MString& NewName );

    private:
        uint64 TicksPerSec;

        uint64 StartupTicks;
        uint64 LastTicks;
        uint64 CurrentTicks;
        uint64 OneSecElasedTicks;

        float DeltaTime;
        uint64 FrameCount;
        uint64 FPS;
        uint64 MaxFPS;

        float TimeScale;

    };
}