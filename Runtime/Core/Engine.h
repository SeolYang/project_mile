#pragma once

#include "SubSystem.h"

namespace Mile
{
   class Logger;
   class ThreadPool;
   class ConfigSystem;
   class Window;
   class ResourceManager;
   class RendererDX11;
   class World;
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
      ThreadPool*         m_threadPool;
      Logger*             m_logger;
      ResourceManager*    m_resourceManager;
      ConfigSystem*       m_configSys;
      Window*             m_window;
      RendererDX11*       m_renderer;
      World*              m_world;

   };
}