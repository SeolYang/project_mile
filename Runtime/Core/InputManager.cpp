#include "Core/InputManager.h"
#include "Core/Logger.h"
#include <utility>

namespace Mile
{
   InputManager::InputManager(Context* context) :
      SubSystem(context)
   {
   }

   InputManager::~InputManager()
   {
   }

   bool InputManager::Init()
   {
      if (SubSystem::Init())
      {
         MELog(GetContext(), TEXT("InputManager"), ELogType::DEBUG, TEXT("InputManager initialized."));
         return true;
      }

      return false;
   }

   void InputManager::MapAction(EInputKey key, const String& actionName)
   {
      for (auto mappingInfo : m_actionMappings)
      {
         if (mappingInfo.second == actionName)
         {
            return;
         }
      }

      m_actionMappings.push_back(std::make_pair(key, actionName));
   }

   void InputManager::BindAction(const String& actionName, EInputEvent inputEvent, ActionCallback callback)
   {
      for (auto mappingInfo : m_actionMappings)
      {
         if (mappingInfo.second == actionName)
         {
            m_actionBindings.push_back(std::make_pair(actionName, std::make_pair(inputEvent, callback)));
         }
      }
   }

   void InputManager::HandleWin32(unsigned int msg, WPARAM wParam, LPARAM lParam)
   {
      //@TODO: Axis 입력도 처리
      BroadcastAction(ConvertWin32Key(wParam), ConvertWin32InputEvent(msg));
   }

   void InputManager::BroadcastAction(EInputKey targetKey, EInputEvent targetEvent)
   {
      for (auto mappingInfo : m_actionMappings)
      {
         if (targetKey == mappingInfo.first)
         {
            for (auto bindingInfo : m_actionBindings)
            {
               if (mappingInfo.second == bindingInfo.first)
               {
                  ActionInputEvent inputEvent = bindingInfo.second;
                  if (inputEvent.first == targetEvent)
                  {
                     inputEvent.second();
                  }
               }
            }
         }
      }
   }
}