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
   Window::Window(Context* context) : SubSystem(context),
      m_handle(nullptr),
      m_resWidth(0), m_resHeight(0),
      m_refreshRate(60.0), m_bIsFullScreen(false)
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
         auto configSys = context->GetSubSystem<ConfigSystem>();

         auto config = configSys->GetConfig(TEXT("Engine"));
         std::wstring windowTitle = String2WString(config.second["Title"]);

         WNDCLASS wndClass = { 0 };
         wndClass.style = CS_OWNDC;
         wndClass.lpfnWndProc = &Mile::WinProc;
         wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
         wndClass.lpszClassName = windowTitle.c_str();
         RegisterClass(&wndClass);

         unsigned int posX = config.second["PosX"];
         unsigned int posY = config.second["PosY"];
         m_resWidth = config.second["ResolutionWidth"];
         m_resHeight = config.second["ResolutionHeight"];

         m_handle = CreateWindow(windowTitle.c_str(), windowTitle.c_str(),
            WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
            posX, posY, m_resWidth, m_resHeight,
            nullptr, nullptr, nullptr, nullptr);

         MELog(context, TEXT("Window"), ELogType::MESSAGE, TEXT("Window Initialized!"), true);
         SubSystem::InitSucceed();
         return true;
      }

      MELog(context, TEXT("Window"), ELogType::WARNING, TEXT("Window already initialized."), true);
      return false;
   }

   void Window::DeInit()
   {
      if (IsInitialized())
      {
         SubSystem::DeInit();
         MELog(GetContext(), TEXT("Window"), ELogType::MESSAGE, TEXT("Window deinitialized."), true);
      }
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
      SetWindowText(m_handle, title.c_str());
   }

   LRESULT WinProc(HWND Handle, unsigned int Msg, WPARAM wParam, LPARAM lParam)
   {
      if (ImGui_ImplWin32_WndProcHandler(Handle, Msg, wParam, lParam))
      {
         return true;
      }

      switch (Msg)
      {
      case WM_DESTROY:
      case WM_CLOSE:
         PostQuitMessage(0);
         return 0;
      }

      InputManager* inputManager = Engine::GetInputManager();
      if (inputManager != nullptr)
      {
         inputManager->HandleWin32(Msg, wParam, lParam);
      }

      return DefWindowProc(Handle, Msg, wParam, lParam);
   }
}