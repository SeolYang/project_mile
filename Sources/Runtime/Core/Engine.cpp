#include "Core/Engine.h"
#include "Core/Context.h"
#include "Core/Logger.h"
#include "Core/Timer.h"
#include "Core/Config.h"
#include "Core/InputManager.h"
#include "Core/Window.h"
#include "Core/Application.h"
#include "GameFramework/World.h"
#include "Resource/ResourceManager.h"
#include "Rendering/RendererPBR.h"
#include "MT/ThreadPool.h"
#include <iostream>
#include <algorithm>

namespace Mile
{
   DEFINE_LOG_CATEGORY(MileEngine);

   Engine* Engine::m_instance = nullptr;
   Engine::Engine(Context* context, Application* app) :
      SubSystem(context), m_bIsRunning(false), m_bShutdownFlag(false),
      m_maxFPS(0), m_targetTimePerFrame(0)
   {
      context->RegisterSubSystem(this);

      m_logger = new Logger(context);
      context->RegisterSubSystem(m_logger);

      m_timer = new Timer(context);
      context->RegisterSubSystem(m_timer);

      m_threadPool = new ThreadPool(context);
      context->RegisterSubSystem(m_threadPool);

      m_resourceManager = new ResourceManager(context);
      context->RegisterSubSystem(m_resourceManager);

      m_configSys = new ConfigSystem(context);
      context->RegisterSubSystem(m_configSys);

      m_inputManager = new InputManager(context);
      context->RegisterSubSystem(m_inputManager);

      m_window = new Window(context);
      context->RegisterSubSystem(m_window);

      m_renderer = new RendererPBR(context, 8);
      context->RegisterSubSystem(m_renderer);

      m_world = new World(context);
      context->RegisterSubSystem(m_world);

      m_app = app;
      if (m_app != nullptr)
      {
         context->RegisterSubSystem(m_app);
      }
   }

   Engine::~Engine()
   {
      DeInit();
   }

   bool Engine::Init()
   {
      if (SubSystem::Init() && m_instance == nullptr)
      {
         m_instance = this;
         // -* Initialize subsystems *-
         Context* context = GetContext();

         // Initialize Logger
         if (!context->GetSubSystem<Logger>()->Init())
         {
            std::cerr << "Engine : Failed to initialize Logger!" << std::endl;
            m_instance = nullptr;
            return false;
         }

         // Initialize Timer
         if (!context->GetSubSystem<Timer>()->Init())
         {
            ME_LOG(MileEngine, Fatal, TEXT("Failed to initialize Timer!"));
            m_instance = nullptr;
            return false;
         }

         // Initialize Thread Pool
         if (!context->GetSubSystem<ThreadPool>()->Init())
         {
            ME_LOG(MileEngine, Fatal, TEXT("Failed to initialize ThreadPool!"));
            m_instance = nullptr;
            return false;
         }

         // Initialize Resource manager
         if (!context->GetSubSystem<ResourceManager>()->Init())
         {
            ME_LOG(MileEngine, Fatal, TEXT("Failed to initialize ResourceManager!"));
            m_instance = nullptr;
            return false;
         }

         // Initialize ConfigSystem
         if (!context->GetSubSystem<ConfigSystem>()->Init())
         {
            ME_LOG(MileEngine, Fatal, TEXT("Failed to initialize ConfigSystem!"));
            m_instance = nullptr;
            return false;
         }

         // Initialize Input Manager
         if (!context->GetSubSystem<InputManager>()->Init())
         {
            ME_LOG(MileEngine, Fatal, TEXT("Failed to initialize InputManager!"));
            m_instance = nullptr;
            return false;
         }

         // Initialize Window Subsystem
         if (!context->GetSubSystem<Window>()->Init())
         {
            ME_LOG(MileEngine, Fatal, TEXT("Failed to initialize Window!"));
            m_instance = nullptr;
            return false;
         }

         if (!context->GetSubSystem<RendererPBR>()->Init((*m_window)))
         {
            ME_LOG(MileEngine, Fatal, TEXT("Failed to initialize Renderer!"));
            m_instance = nullptr;
            return false;
         }

         // Initialize World
         if (!context->GetSubSystem<World>()->Init())
         {
            ME_LOG(MileEngine, Fatal, TEXT("Failed to initialize World!"));
            m_instance = nullptr;
            return false;
         }

         Application* app = context->GetSubSystem<Application>();
         if (app == nullptr || !app->Init())
         {
            ME_LOG(MileEngine, Fatal, TEXT("Failed to initialize Application!"));
            m_instance = nullptr;
            return false;
         }

         /** Load Engine Config  */
         this->LoadConfig();

         ME_LOG(MileEngine, Log, TEXT("Engine initialized."));
         SubSystem::InitSucceed();
         return true;
      }

      return false;
   }

   void Engine::DeInit()
   {
      if (IsInitialized())
      {
         m_bIsRunning = false;
         m_logger = nullptr;
         m_timer = nullptr;
         m_threadPool = nullptr;
         m_resourceManager = nullptr;
         m_configSys = nullptr;
         m_window = nullptr;
         m_renderer = nullptr;
         m_world = nullptr;
         m_app = nullptr;
         SubSystem::DeInit();
      }
   }

   int Engine::Execute()
   {
      m_bIsRunning = true;
      m_world->Start();
      while (m_bIsRunning)
      {
         OPTICK_FRAME("MainThread");
         if (m_bShutdownFlag)
         {
            ShutDown();
         }
         else
         {
            OPTICK_EVENT();
            m_timer->BeginFrame();

            this->Update();
            m_app->Update();
            /** @todo 나중에 audio/physics 시스템이 추가되면 서로 다른 스레드에서 처리하도록 하기 */
            auto mainRenderTask = m_threadPool->AddTask(
               [this]() 
               {
                  this->m_renderer->Render(*this->m_world); 
               }); 
            mainRenderTask.get();

            m_app->RenderIMGUI();
            m_renderer->Present();

            m_timer->PreEndFrame();

            auto deltaTimeNS = m_timer->GetDeltaTimeNS();
            if (deltaTimeNS < m_targetTimePerFrame)
            {
               auto yieldTime = std::chrono::nanoseconds(m_targetTimePerFrame - deltaTimeNS);
               auto end = std::chrono::high_resolution_clock::now() + yieldTime;
               while (std::chrono::high_resolution_clock::now() < end)
               {
                  std::this_thread::yield();
               }
            }

            m_timer->PostEndFrame();
         }
      }

      return 0;
   }

   void Engine::Update()
   {
      OPTICK_EVENT();
      // Update subsystems
      m_window->Update();
      m_world->Update();
   }

   void Engine::ShutDown()
   {
      ME_LOG(MileEngine, Log, TEXT("Engine shutting down."));
      m_bIsRunning = false;
   }

   Engine* Engine::GetInstance()
   {
      return m_instance;
   }

   Logger* Engine::GetLogger()
   {
      return (m_instance != nullptr) ? m_instance->m_logger : nullptr;
   }

   Timer* Engine::GetTimer()
   {
      return (m_instance != nullptr) ? m_instance->m_timer : nullptr;
   }

   ThreadPool* Engine::GetThreadPool()
   {
      return (m_instance != nullptr) ? m_instance->m_threadPool : nullptr;
   }

   ResourceManager* Engine::GetResourceManager()
   {
      return (m_instance != nullptr) ? m_instance->m_resourceManager : nullptr;
   }

   ConfigSystem* Engine::GetConfigSystem()
   {
      return (m_instance != nullptr) ? m_instance->m_configSys : nullptr;
   }

   InputManager* Engine::GetInputManager()
   {
      return (m_instance != nullptr) ? m_instance->m_inputManager : nullptr;
   }

   Window* Engine::GetWindow()
   {
      return (m_instance != nullptr) ? m_instance->m_window : nullptr;
   }

   RendererDX11* Engine::GetRenderer()
   {
      return (m_instance != nullptr) ? m_instance->m_renderer : nullptr;
   }

   World* Engine::GetWorld()
   {
      return (m_instance != nullptr) ? m_instance->m_world : nullptr;
   }

   void Engine::LoadConfig()
   {
      if (m_configSys != nullptr)
      {
         if (m_configSys->LoadConfig(ENGINE_CONFIG))
         {
            auto& engineConfig = m_configSys->GetConfig(ENGINE_CONFIG);
            unsigned int maxFPS = GetValueSafelyFromJson(engineConfig.second, ENGINE_CONFIG_MAX_FPS, UPPER_BOUND_OF_ENGINE_FPS);
            SetMaxFPS(maxFPS);
            ME_LOG(MileEngine, ELogVerbosity::Log, TEXT("Engine configurations loaded."));
            return;
         }
      }

      ME_LOG(MileEngine, ELogVerbosity::Fatal, TEXT("Failed to load Engine default config!"));
   }

   void Engine::SaveConfig()
   {
      if (m_configSys != nullptr)
      {
         if (m_configSys->LoadConfig(ENGINE_CONFIG))
         {
            auto& engineConfig = m_configSys->GetConfig(ENGINE_CONFIG);
            engineConfig.second[ENGINE_CONFIG_MAX_FPS] = m_maxFPS;
            if (m_configSys->SaveConfig(ENGINE_CONFIG))
            {
               ME_LOG(MileEngine, ELogVerbosity::Log, TEXT("Engine configurations saved."));
               return;
            }
         }
      }

      ME_LOG(MileEngine, ELogVerbosity::Fatal, TEXT("Failed to save Engine configurations!"));
   }

   long long Engine::GetCurrentFPS() const
   {
      if (m_timer != nullptr)
      {
         return m_timer->GetFPS();
      }

      return 0;
   }
}