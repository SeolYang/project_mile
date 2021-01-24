#pragma once
#include "Core/Logger.h"
#include <functional>

namespace Mile
{
   DECLARE_LOG_CATEGORY_EXTERN(MileInputManager, Log);

   enum class MEAPI EInputEvent
   {
      Pressed,
      Released,
      Axis,
      Unknown
   };

   enum class MEAPI EInputKey
   {
      /* Keyboard */
      W,
      A,
      S,
      D,
      UP,
      Down,
      Left,
      Right,

      /* Mouse */

      Unknown
   };

   static EInputKey ConvertWin32Key(WPARAM wParam)
   {
      switch (wParam)
      {
      default:
         return EInputKey::Unknown;

      case VK_UP:
         return EInputKey::UP;

      case VK_DOWN:
         return EInputKey::Down;

      case VK_LEFT:
         return EInputKey::Left;

      case VK_RIGHT:
         return EInputKey::Right;

      case 'W':
      case 'w':
         return EInputKey::W;

      case 'A':
      case 'a':
         return EInputKey::A;

      case 'S':
      case 's':
         return EInputKey::S;

      case 'D':
      case 'd':
         return EInputKey::D;
      }
   }

   static EInputEvent ConvertWin32InputEvent(unsigned int msg)
   {
      switch (msg)
      {
      default:
         return EInputEvent::Unknown;

      case WM_KEYDOWN:
         return EInputEvent::Pressed;

      case WM_KEYUP:
         return EInputEvent::Released;
      }
   }

   /* (키, ID) 맵핑 / (ID, CALLBACK) 바인딩 **/
   class Entity;
   class MEAPI InputManager : public SubSystem
   {
   public:
      using ActionCallback = std::function<void()>;
      using ActionInputMapping = std::pair<EInputKey, String>;
      using ActionInputEvent = std::pair<EInputEvent, ActionCallback>;
      using ActionInputBinding = std::pair<String, ActionInputEvent>;

   public:
      InputManager(Context* context);
      virtual ~InputManager();

      virtual bool Init() override;
      virtual void DeInit() override;

      void MapAction(EInputKey key, const String& actionName);
      void BindAction(const String& actionName, EInputEvent inputEvent, ActionCallback callback);

   //   void MapAxis(EInputKey key, const String& eventID, float scale);
   //   void BindAxis(const String& eventID, Entity* entity, AxisCallback callback);

      void HandleWin32(unsigned int msg, WPARAM wParam, LPARAM lParam);

   private:
      void BroadcastAction(EInputKey targetKey, EInputEvent targetEvent);
   //   void BroadcastAxis(EInputKey targetKey, float axisValue);

   private:
      std::vector<ActionInputMapping> m_actionMappings;
      std::vector<ActionInputBinding> m_actionBindings;

   };
}