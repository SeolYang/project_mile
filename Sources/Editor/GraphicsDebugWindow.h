#pragma once
#include "Core/CoreMinimal.h"

namespace Mile
{
   namespace Editor
   {
      enum class EGraphicsDebug
      {
         Position,
         Albedo,
         Normal,
         Depth,
         SSAO,
         Lighting
      };

      class GraphicsDebugWindow
      {
      public:
         GraphicsDebugWindow();
         void OnGUI();

      private:
         EGraphicsDebug m_selected;

      };
   }
}