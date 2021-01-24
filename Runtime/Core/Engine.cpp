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
#include "Rendering/RendererDX11.h"
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

      m_renderer = new RendererDX11(context);
      context->RegisterSubSystem(m_renderer);

      m_world = new World(context);
      context->RegisterSubSystem(m_world);

      m_app = app;
      if (m_app != nullptr)
      {
         context->RegisterSubSystem(m_app);
      }
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

         if (!context->GetSubSystem<RendererDX11>()->Init())
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

         auto engineConfig = m_configSys->GetConfig(TEXT("Engine"));
         m_maxFPS = std::clamp(static_cast<unsigned int>(engineConfig.second["MaxFPS"]), LOWER_BOUND_OF_ENGINE_FPS, UPPER_BOUND_OF_ENGINE_FPS);
         m_targetTimePerFrame = static_cast<long long>((1.0 / static_cast<double>(m_maxFPS)) * 1000.0);

         SubSystem::InitSucceed();
         ME_LOG(MileEngine, Log, TEXT("Engine initialized."));
         return true;
      }

      return false;
   }

   int Engine::Execute()
   {
      m_bIsRunning = true;
      m_world->Start();
      while (m_bIsRunning)
      {
         if (m_bShutdownFlag)
         {
            ShutDown();
         }
         else
         {
            m_timer->BeginFrame();

            this->Update();
            m_app->Update();
            m_renderer->Render();
            m_app->RenderIMGUI();
            m_renderer->Present();

            m_timer->PreEndFrame();

            auto deltaTimeMS = m_timer->GetDeltaTimeMS();
            if (deltaTimeMS < m_targetTimePerFrame)
            {
               std::this_thread::sleep_for(std::chrono::milliseconds(m_targetTimePerFrame - deltaTimeMS));
               m_timer->PostEndFrame();
            }
         }
      }

      return 0;
   }

   void Engine::Update()
   {
      // Update subsystems
      m_window->Update();
      m_world->Update();
   }

   void Engine::ShutDown()
   {
      ME_LOG(MileEngine, Log, TEXT("Engine shutting down."));
      Context* context = GetContext();
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
}
