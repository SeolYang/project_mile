#pragma once
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileTimer, Log);

   /**
    * @brief	Timer 서브 시스템은 엔진 내에서 시간과 관련된 정보를 측정하거나 제공해주는 클래스 입니다.
    */
   class MEAPI Timer : public SubSystem
   {
   public:
      Timer(Context* context);
      virtual ~Timer();

      virtual bool Init() override;
      virtual void DeInit() override;

      void BeginFrame();
      /**
       * @brief   남는 시간만큼 CPU 가 sleep 하기 이전에 호출
       */
      void PreEndFrame();

      /**
       * @brief   CPU가 sleep에서 깨어났을때 호출(설정한 프레임당 시간보다 프레임 처리가 더 빨리 끝났을때 delta time 등을 다시 계산)
       */
      void PostEndFrame();

      /**
       * @brief	이전 프레임과 현재 프레임사이에 시간 차이를 '초' 단위로 반환 해줍니다.
       * @return 프레임간 지난 시간(초)
       */
      float GetDeltaTime() const
      {
         return GetDeltaTimeNS() * 0.000000001f;
      }

      /**
      * @brief	이전 프레임과 현재 프레임사이에 시간 차이를 'ms' 단위로 반환 해줍니다.
      * @return 프레임간 지난 시간(ms)
      */
      double GetDeltaTimeMS() const
      {
         return (GetDeltaTimeNS() * 0.000001);
      }

      long long GetDeltaTimeNS() const
      {
         return ((m_deltaTime).count());
      }

      /**
       * @brief	엔진이 초당 몇개의 프레임을 수행했는지 반환합니다.
       * @return 초당 프레임 수
       */
      long long GetFPS() const
      {
         return m_framePerSec;
      }

   private:
      std::chrono::steady_clock::time_point m_frameBeginTime;
      std::chrono::steady_clock::time_point m_frameEndTime;
      std::chrono::nanoseconds m_deltaTime;

      std::chrono::steady_clock::time_point m_frameMeasureBeginTime;
      long long   m_frameCount;
      long long   m_framePerSec;

   };
}