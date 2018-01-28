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
         return GetDeltaTimeMS( ) * 0.001f;
      }

      long long GetDeltaTimeMS( ) const
      {
         return ( std::chrono::duration_cast< std::chrono::milliseconds >( m_deltaTime ).count( ) );
      }

      long long GetDeltaTimeNS( ) const
      {
         auto deltaTime = m_frameEndTime - m_frameBeginTime;
         return ( ( m_deltaTime ).count( ) );
      }

      long long GetFPS( ) const
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