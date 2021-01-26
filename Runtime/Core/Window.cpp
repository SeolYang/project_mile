#include "Core/Window.h"
#include "Core/Context.h"
#include "Core/Engine.h"
#include "Core/Logger.h"
#include "Core/Config.h"
#include "Core/InputManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace Mile
{
   DEFINE_LOG_CATEGORY(MileWindow);

   Window::Window(Context* context) : SubSystem(context),
      m_handle(nullptr),
      m_title(TEXT("Mile")),
      m_resWidth(DEFAULT_WINDOW_RES_WIDTH), m_resHeight(DEFAULT_WINDOW_RES_HEIGHT),
      m_windowStyle(EWindowStyle::Windowed)
   {
   }

   Window::~Window()
   {
      DeInit();
   }

   bool Window::Init()
   {
      Context* context = GetContext();
      if (SubSystem::Init())
      {
         LoadConfig();
         auto configSys = context->GetSubSystem<ConfigSystem>();

         WNDCLASS wndClass = { 0 };
         wndClass.style = CS_OWNDC;
         wndClass.lpfnWndProc = &Mile::WinProc;
         wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
         wndClass.lpszClassName = m_title.c_str();
         RegisterClass(&wndClass);

         int systemResX = GetSystemMetrics(SM_CXSCREEN);
         int systemResY = GetSystemMetrics(SM_CYSCREEN);

         unsigned int posX = (systemResX - m_resWidth) / 2;
         unsigned int posY = (systemResY - m_resHeight) / 2;

         RECT windowRect;
         windowRect.left = 0;
         windowRect.right = m_resWidth;
         windowRect.bottom = m_resHeight;
         windowRect.top = 0;

         auto style = WS_POPUP;
         auto exStyle = WS_EX_APPWINDOW;
         switch (m_windowStyle)
         {
         case EWindowStyle::Windowed:
            style |= WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
            if (AdjustWindowRectEx(&windowRect, style, false, exStyle) == 0)
            {
               return false;
            }

            break;

         case EWindowStyle::Borderless:
            break;

         case EWindowStyle::FullScreen:
            posX = 0;
            posY = 0;
            windowRect.right = systemResX;
            windowRect.bottom = systemResY;
            WinSetVideoMode(systemResX, systemResY, 32);
            ShowCursor(false);
            break;
         }

         m_handle = CreateWindowEx(
            exStyle,
            m_title.c_str(), m_title.c_str(),
            style,
            posX, posY,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr, nullptr, nullptr, nullptr);

         ShowWindow(m_handle, SW_SHOW);

         ME_LOG(MileWindow, Log, TEXT("Window Initialized."));
         SubSystem::InitSucceed();
         return true;
      }

      ME_LOG(MileWindow, Warning, TEXT("Window already initialized!"));
      return false;
   }

   void Window::DeInit()
   {
      if (IsInitialized())
      {
         SaveConfig();
         DestroyWindow(m_handle);
         ME_LOG(MileWindow, Log, TEXT("Window deinitialized."));
         SubSystem::DeInit();
      }
   }

   Vector2 Window::GetWindowResolution() const
   {
      RECT windowRect;
      windowRect.left = 0;
      windowRect.right = m_resWidth;
      windowRect.bottom = m_resHeight;
      windowRect.top = 0;

      if (AdjustWindowRectEx(&windowRect, WindowStyleToFlags(m_windowStyle), false, WS_EX_APPWINDOW) == 0)
      {
         return Vector2();
      }

      return Vector2(
         windowRect.right - windowRect.left,
         windowRect.bottom - windowRect.top);
   }

   float Window::GetWindowAspectRatio() const
   {
      Vector2 windowResolution = GetWindowResolution();
      return windowResolution.x / windowResolution.y;
   }

   void Window::Update()
   {
      Context* context = GetContext();
      MSG msg = { 0 };
      if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
      {
         auto message = msg.message;
         if (message == WM_QUIT || message == WM_CLOSE || message == WM_DESTROY)
         {
            // Dummy flag for shutting down after 1 frame
            context->GetSubSystem<Engine>()->ShutDownFlagEnable();
         }

         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }

   void Window::SetTitle(const String& title)
   {
      m_title = title;

      if (m_handle != nullptr)
      {
         SetWindowText(m_handle, title.c_str());
      }
   }

   void Window::_OnWindowResize(unsigned int width, unsigned int height)
   {
      if (m_handle != nullptr)
      {
         RECT clientRect;
         GetClientRect(m_handle, &clientRect);
         m_resWidth = width;
         m_resHeight = height;
         ME_LOG(MileWindow, Log, TEXT("Window Resized! Window Resolution : %d x %d"), m_resWidth, m_resHeight);

         OnWindowResize.Broadcast(m_resWidth, m_resHeight);
      }
   }

   void Window::_OnWindowMinimized()
   {
      OnWindowMinimized.Broadcast();
   }

   void Window::LoadConfig()
   {
      ConfigSystem* configSys = Engine::GetConfigSystem();
      if (configSys != nullptr)
      {
         if (configSys->LoadConfig(WINDOW_CONFIG))
         {
            auto& config = configSys->GetConfig(WINDOW_CONFIG);
            m_resWidth = GetValueSafelyFromJson(config.second, WINDOW_CONFIG_RESOLUTION_WIDTH, m_resWidth);
            m_resHeight = GetValueSafelyFromJson(config.second, WINDOW_CONFIG_RESOLUTION_HEIGHT, m_resHeight);

            std::string title;
            title = GetValueSafelyFromJson<std::string>(config.second, WINDOW_CONFIG_TITLE, DEFAULT_WINDOW_TITLE);
            m_title = String2WString(title);

            m_windowStyle = IndexToWindowStyle(GetValueSafelyFromJson(config.second, WINDOW_CONFIG_WINDOW_STYLE, static_cast<unsigned int>(m_windowStyle)));
            ME_LOG(MileWindow, Log, TEXT("Window configuration loaded!"));
            return;
         }
      }

      ME_LOG(MileWindow, Fatal, TEXT("Failed to load window config!"));
   }

   void Window::SaveConfig()
   {
      ConfigSystem* configSys = Engine::GetConfigSystem();
      if (configSys != nullptr)
      {
         if (configSys->LoadConfig(WINDOW_CONFIG))
         {
            auto& config = configSys->GetConfig(WINDOW_CONFIG);
            config.second[WINDOW_CONFIG_RESOLUTION_WIDTH] = m_resWidth;
            config.second[WINDOW_CONFIG_RESOLUTION_HEIGHT] = m_resHeight;
            config.second[WINDOW_CONFIG_TITLE] = WString2String(m_title);
            config.second[WINDOW_CONFIG_WINDOW_STYLE] = static_cast<UINT32>(m_windowStyle);

            if (configSys->SaveConfig(WINDOW_CONFIG))
            {
               ME_LOG(MileWindow, Log, TEXT("Window configuration saved!"));
               return;
            }
         }
      }

      ME_LOG(MileWindow, Fatal, TEXT("Failed to save window config!"));
   }

   LRESULT WinProc(HWND Handle, unsigned int Msg, WPARAM wParam, LPARAM lParam)
   {
      Window* window = Engine::GetWindow();
      InputManager* inputManager = Engine::GetInputManager();

      switch (Msg)
      {
      case WM_DESTROY:
      case WM_CLOSE:
         PostQuitMessage(0);
         return 0;
      case WM_SIZE:
         if (window != nullptr)
         {
            switch (wParam)
            {
            case SIZE_MINIMIZED:
               window->_OnWindowMinimized();
               break;
            default:
               /* Client Area Size! **/
               window->_OnWindowResize(LOWORD(lParam), HIWORD(lParam));
               break;
            }
         }
         break;
      }

      if (inputManager != nullptr)
      {
         inputManager->HandleWin32(Msg, wParam, lParam);
      }

      ImGui_ImplWin32_WndProcHandler(Handle, Msg, wParam, lParam);

      return DefWindowProc(Handle, Msg, wParam, lParam);
   }
}