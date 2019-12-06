#pragma once

#include "SubSystem.h"

namespace Mile
{
   class Logger;
   class Timer;
   class ThreadPool;
   class ConfigSystem;
   class Window;
   class ResourceManager;
   class RendererDX11;
   class World;
   /**
    * @brief	모든 Subsystem 들의 root 역할을 합니다. Subsystem 들의 초기화, 업데이트 그리고 할당 해제를 당담하는 클래스 입니다.
    */
   class MEAPI Engine : public SubSystem
   {
   public:
      Engine( Context* context );

      /**
      * @brief Engine의 Subsystem 들을 초기화합니다.
      * @return 초기화 성공 여부
      */
      virtual bool Init( ) override;

      int Execute( );

      /**
      * @brief Engine의 Subsystem 들을 업데이트합니다.
      */
      void Update( );

      /**
      * @brief Engine의 Subsystem 들을 해제합니다.
      */
      void ShutDown( );
      void ShutDownFlagEnable( ) { m_bShutdownFlag = true; }

      bool IsRunning( ) const { return m_bIsRunning; }

   private:
      bool                m_bIsRunning;
      bool                m_bShutdownFlag;
      unsigned int        m_maxFPS;
      long long           m_frameTime;
      Logger*             m_logger;
      Timer*              m_timer;
      ThreadPool*         m_threadPool;
      ResourceManager*    m_resourceManager;
      ConfigSystem*       m_configSys;
      Window*             m_window;
      RendererDX11*       m_renderer;
      World*              m_world;

   };
}