#pragma once
#include "Core/SubSystem.h"
#include "Math/Vector2.h"
#include <Windows.h>

namespace Mile
{
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
       * @brief	설정된 윈도우 창 해상도의 너비를 반환합니다.
       * @return	 윈도우 창 해상도의 너비
       */
      unsigned int GetResWidth() const { return m_resWidth; }

      /**
       * @brief	설정된 윈도우 창 해상도의 높이를 반환합니다.
       * @return	 윈도우 창 해상도의 높이
       */
      unsigned int GetResHeight() const { return m_resHeight; }

      /**
       * @brief 설정된 윈도우 창의 해상도를 반환합니다.
       * @return 윈도우 창의 해상도
       */
      Vector2 GetResolution() const { return Vector2(static_cast<float>(m_resWidth), static_cast<float>(m_resHeight)); }
      /**
       * @brief 설정된 윈도우 창 해상도의 종횡비를 반환합니다.
       * @return 윈도우 창 해상도의 종횡비
       */
      float GetAspectRatio() const { return (m_resWidth / static_cast<float>(m_resHeight)); }

      /**
       * @brief	설정된 윈도우 창의 재생빈도수를 반환합니다.
       * @return 윈도우 창의 재생빈도수
       */
      Float GetRefreshRate() const { return m_refreshRate; }

      /**
       * @brief 윈도우 창이 풀 스크린 모드인지 확인합니다.
       * @return	풀 스크린 모드 여부
       */
      bool IsFullScreen() const { return m_bIsFullScreen; }

      virtual void Update() override;

      void SetTitle(const String& title);

   private:
      HWND m_handle;
      unsigned int m_resWidth;
      unsigned int m_resHeight;
      Float        m_refreshRate;

      bool         m_bIsFullScreen;
   };

   LRESULT CALLBACK WinProc(HWND Handle, unsigned int Msg, WPARAM wParam, LPARAM lParam);
}
