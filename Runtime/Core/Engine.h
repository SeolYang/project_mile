#pragma once
#include "Core/Logger.h"

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileEngine, ELogVerbosity::Log);

   constexpr unsigned int LOWER_BOUND_OF_ENGINE_FPS = 1;
   constexpr unsigned int UPPER_BOUND_OF_ENGINE_FPS = 300;

   class Timer;
   class ThreadPool;
   class ConfigSystem;
   class InputManager;
   class Window;
   class ResourceManager;
   class RendererDX11;
   class Application;
   class World;
   /**
    * @brief	모든 Subsystem 들의 root 역할을 합니다. 엔진에서 사용할 Subsystem 들의 초기화, 업데이트 그리고 할당 해제를 당담하는 클래스 입니다.
    */
   class MEAPI Engine : public SubSystem
   {
   public:
      Engine(Context* context, Application* app);
      virtual ~Engine();

      /**
      * @brief Engine의 Subsystem 들을 초기화합니다.
      * @return 초기화 성공 여부
      */
      virtual bool Init() override;
      virtual void DeInit() override;

      int Execute();

      /**
      * @brief Engine의 Subsystem 들을 업데이트합니다.
      */
      void Update();

      /**
      * @brief Engine의 Subsystem 들을 해제합니다.
      */
      void ShutDown();
      void ShutDownFlagEnable() { m_bShutdownFlag = true; }

      bool IsRunning() const { return m_bIsRunning; }

      static Engine* GetInstance();
      static Logger* GetLogger();
      static Timer* GetTimer();
      static ThreadPool* GetThreadPool();
      static ResourceManager* GetResourceManager();
      static ConfigSystem* GetConfigSystem();
      static InputManager* GetInputManager();
      static Window* GetWindow();
      static RendererDX11* GetRenderer();
      static World* GetWorld();

   private:
      static Engine*    m_instance;
      bool              m_bIsRunning;
      bool              m_bShutdownFlag;
      unsigned int      m_maxFPS;
      long long         m_targetTimePerFrame;
      Logger*           m_logger;
      Timer*            m_timer;
      ThreadPool*       m_threadPool;
      ResourceManager*  m_resourceManager;
      ConfigSystem*     m_configSys;
      InputManager*     m_inputManager;
      Window*           m_window;
      RendererDX11*     m_renderer;
      World*            m_world;
      Application*      m_app;

   };
}
