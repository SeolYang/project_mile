#include "Core/Timer.h"
#include "Core/Logger.h"

namespace Mile
{
   Timer::Timer(Context* context) :
      m_frameCount(0), m_framePerSec(0), m_deltaTime(),
      SubSystem(context)
   {
   }

   Timer::~Timer()
   {
      DeInit();
   }

   bool Timer::Init()
   {
      Context* context = GetContext();
      if (SubSystem::Init())
      {
         m_frameBeginTime = m_frameEndTime = std::chrono::steady_clock::now();
         m_deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>
            (m_frameEndTime - m_frameBeginTime);
         m_frameCount = 0;

         MELog(context, TEXT("Timer"), ELogType::MESSAGE, TEXT("Timer initialized."));
         SubSystem::InitSucceed();
         return true;
      }

      MELog(context, TEXT("Timer"), ELogType::FATAL, TEXT("Failed to initialize Timer."));
      return false;
   }

   void Timer::DeInit()
   {
      if (IsInitialized())
      {
         SubSystem::DeInit();
         MELog(GetContext(), TEXT("Timer"), ELogType::MESSAGE, TEXT("Timer deinitialized."));
      }
   }

   void Timer::BeginFrame()
   {
      m_frameBeginTime = std::chrono::steady_clock::now();
      if (m_frameCount == 0)
      {
         m_frameMeasureBeginTime = m_frameBeginTime;
      }
   }

   void Timer::PreEndFrame()
   {
      ++m_frameCount;
      m_frameEndTime = std::chrono::steady_clock::now();
      m_deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(m_frameEndTime - m_frameBeginTime);

      auto dt = m_frameEndTime - m_frameMeasureBeginTime;
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(dt);
      if (duration.count() >= 1000)
      {
         m_framePerSec = m_frameCount;
         m_frameCount = 0;
      }
   }

   void Timer::PostEndFrame()
   {
      m_frameEndTime = std::chrono::steady_clock::now();
      m_deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(m_frameEndTime - m_frameBeginTime);

      auto dt = m_frameEndTime - m_frameMeasureBeginTime;
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(dt);
      if (duration.count() >= 1000)
      {
         m_framePerSec = m_frameCount;
         m_frameCount = 0;
      }
   }
}