#pragma once
#include "Core/SubSystem.h"
#include <chrono>

namespace Mile
{
   /**
    * @brief	Timer 서브 시스템은 엔진 내에서 시간과 관련된 정보를 측정하거나 제공해주는 클래스 입니다.
    */
   class MEAPI Timer : public SubSystem
   {
   public:
      Timer(Context* context);
      ~Timer();

      virtual bool Init() override;

      void BeginFrame();
      void EndFrame();

      /**
       * @brief	이전 프레임과 현재 프레임사이에 시간 차이를 '초' 단위로 반환 해줍니다.
       * @return 프레임간 지난 시간(초)
       */
      float GetDeltaTime() const
      {
         return GetDeltaTimeMS() * 0.001f;
      }

      /**
      * @brief	이전 프레임과 현재 프레임사이에 시간 차이를 'ms' 단위로 반환 해줍니다.
      * @return 프레임간 지난 시간(ms)
      */
      long long GetDeltaTimeMS() const
      {
         return (std::chrono::duration_cast<std::chrono::milliseconds>(m_deltaTime).count());
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