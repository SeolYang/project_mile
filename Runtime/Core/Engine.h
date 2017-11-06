#pragma once

#include "SubSystem.h"

namespace Mile
{
   class ConfigSystem;
   class Window;
   class ResourceManager;
   class World;
   class MEAPI Engine : public SubSystem
   {
   public:
      Engine( Context* context );
      virtual ~Engine( ) { ShutDown( ); }

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

      bool IsRunning( ) const { return m_bIsRunning; }

   private:
      bool                m_bIsRunning;
      ConfigSystem*       m_configSys;
      Window*             m_window;
      ResourceManager*    m_resourceManager;
      World*              m_world;

   };
}