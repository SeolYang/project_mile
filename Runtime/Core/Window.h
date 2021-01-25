#pragma once
#include "Core/Logger.h"
#include "Core/Delegate.h"
#include "Math/Vector2.h"
#include <Windows.h>

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileWindow, Log);
   DECLARE_MULTICAST_DELEGATE_Params(OnWindowResizeMulticastDelegate, unsigned int, unsigned int);
   DECLARE_MULTICAST_DELEGATE(OnWindowMinimizedMulticastDelegate);
   DECLARE_DELEGATE_Params(OnWindowResizeDelegate, unsigned int, unsigned int);
   DECLARE_DELEGATE(OnWindowMinimizedDelegate);

   enum class EWindowStyle
   {
      FullScreen, // 0
      Borderless, // 1
      Windowed    // 2 and else
   };

   static EWindowStyle IndexToWindowStyle(unsigned int idx)
   {
      switch (idx)
      {
      case 0:
         return EWindowStyle::FullScreen;
      case 1:
         return EWindowStyle::Borderless;
      case 2:
      default:
         return EWindowStyle::Windowed;
      }
   }

   static unsigned long WindowStyleToFlags(EWindowStyle style)
   {
      auto flags = WS_POPUP;
      switch (style)
      {
      case Mile::EWindowStyle::FullScreen:
         break;
      case Mile::EWindowStyle::Borderless:
         break;
      case Mile::EWindowStyle::Windowed:
         flags |= WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
         break;
      }

      return flags;
   }

   static void WinSetVideoMode(int width, int height, int bpp)
   {
      DEVMODE Mode;
      EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &Mode);
      Mode.dmBitsPerPel = bpp;
      Mode.dmPelsWidth = width;
      Mode.dmPelsHeight = height;
      Mode.dmSize = sizeof(Mode);
      Mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

      ChangeDisplaySettings(&Mode, CDS_FULLSCREEN);
   }

   static void WinRestoreVideoMode()
   {
      ChangeDisplaySettings(NULL, 0);
   }

   /**
    * @brief	화면이 보여질 윈도우를 나타내는 서브 시스템 클래스입니다.
    */
   class MEAPI Window : public SubSystem
   {
   public:
      Window(Context* context);
      virtual ~Window();

      virtual bool Init() override;
      virtual void DeInit() override;

      /**
       * @brief	Win32의 raw handle 포인터를 반환합니다.
       * @return	핸들 포인터
       */
      void* GetHandle() { return m_handle; }

      /**
       * @brief 설정된 윈도우의 client 영역의 해상도를 반환합니다.
       * @return 윈도우의 client 영역의 해상도
       */
      Vector2 GetResolution() const { return Vector2(static_cast<float>(m_resWidth), static_cast<float>(m_resHeight)); }

      /**
       * @brief 설정된 윈도우의 client 영역의 해상도 종횡비를 반환합니다.
       * @return 윈도우의 client 영역의 해상도 종횡비
       */
      float GetAspectRatio() const { return (m_resWidth / static_cast<float>(m_resHeight)); }

      /**
      * @brief 설정된 윈도우 창의 해상도를 반환합니다.
      * @return 윈도우의 client 영역의 해상도
      */
      Vector2 GetWindowResolution() const;
      float GetWindowAspectRatio() const;

      /**
       * @brief	설정된 윈도우 창의 재생빈도수를 반환합니다.
       * @return 윈도우 창의 재생빈도수
       */
      Float GetRefreshRate() const { return m_refreshRate; }

      /**
       * @brief 윈도우 창이 풀 스크린 모드인지 확인합니다.
       * @return	풀 스크린 모드 여부
       */
      bool IsFullScreen() const { return m_windowStyle == EWindowStyle::FullScreen || m_windowStyle == EWindowStyle::Borderless; }

      virtual void Update() override;

      void SetTitle(const String& title);

      void _OnWindowResize(unsigned int width, unsigned int height);
      void _OnWindowMinimized();

   private:
      HWND m_handle;
      unsigned int m_resWidth;
      unsigned int m_resHeight;
      Float        m_refreshRate;

      EWindowStyle m_windowStyle;

   public:
      OnWindowResizeMulticastDelegate OnWindowResize;
      OnWindowMinimizedMulticastDelegate OnWindowMinimized;

   };

   LRESULT CALLBACK WinProc(HWND Handle, unsigned int Msg, WPARAM wParam, LPARAM lParam);
}
