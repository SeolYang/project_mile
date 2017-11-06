#pragma once
#include <Windows.h>
#include "SubSystem.h"

namespace Mile
{
   class MEAPI Window : public SubSystem
   {
   public:
      Window( Context* context );
      virtual ~Window( );

      virtual bool Init( ) override;

      void* GetHandle( ) { return m_handle; }

      unsigned int GetResWidth( ) const { return m_resWidth; }
      unsigned int GetResHeight( ) const { return m_resHeight; }
      std::pair<unsigned int, unsigned int> GetResolution( ) const { return std::make_pair( m_resWidth, m_resHeight ); }

      Float GetRefreshRate( ) const { return m_refreshRate; }

      bool IsFullScreen( ) const { return m_bIsFullScreen; }

      virtual void Update( ) override;

   private:
      HWND m_handle;
      unsigned int m_resWidth;
      unsigned int m_resHeight;
      Float        m_refreshRate;

      bool         m_bIsFullScreen;
   };

   LRESULT CALLBACK WinProc( HWND Handle, unsigned int Msg, WPARAM wParam, LPARAM lParam );
}
