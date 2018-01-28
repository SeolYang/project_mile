#pragma once

#include "SubSystem.h"
#include <chrono>

namespace Mile
{
   class MEAPI Timer : public SubSystem
   {
   public:
      Timer( Context* context );
      ~Timer( );

      virtual bool Init( ) override;

      virtual void Update( ) override;

      void BeginFrame( );
      void EndFrame( );

      float GetDeltaTime( ) const
      {
         auto deltaTime = m_frameEndTime - m_frameBeginTime;
         return ( std::chrono::duration_cast< std::chrono::duration<float> >( deltaTime ).count( ) );
      }

      long long GetDeltaTimeMS( ) const
      {
         auto deltaTime = m_frameEndTime - m_frameBeginTime;
         return ( std::chrono::duration_cast< std::chrono::milliseconds >( deltaTime ).count( ) );
      }

      long long GetDeltaTimeNS( ) const
      {
         auto deltaTime = m_frameEndTime - m_frameBeginTime;
         return ( std::chrono::duration_cast< std::chrono::nanoseconds >( deltaTime ).count( ) );
      }

      long long GetFPS( ) const
      {
         return m_framePerSec;
      }

   private:
      std::chrono::steady_clock::time_point m_frameBeginTime;
      std::chrono::steady_clock::time_point m_frameEndTime;

      std::chrono::steady_clock::time_point m_frameMeasureBeginTime;
      long long   m_frameCount;
      long long   m_framePerSec;

   };
}